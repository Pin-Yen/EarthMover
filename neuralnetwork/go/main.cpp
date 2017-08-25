#include <assert.h>
#include <iostream>
#include <string>
#include <cstdlib>

#include "../../timer.h"
#include "../../go/displayboard.cpp"

#include "../../lib/tiny-dnn/tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;

int nnPredict(network<sequential> *nn,
              const int board[19][19], bool blackTurn) {
  int input[1083] = {0};

  // Input feature.
  for (int r = 0; r < 19; ++r) {
    for (int c = 0; c < 19; ++c) {
      switch (board[r][c]) {
        case 0:  // Empty.
          input[361 * 2 + r * 19 + c] = 1;
        case -1:  // Black chess.
          if (blackTurn) {
            input[r * 19 + c] = 1;
          } else {
            input[361 + r * 19 + c] = 1;
          }
        case 1:
          if (blackTurn) {
            input[361 + r * 19 + c] = 1;
          } else {
            input[r * 19 + c] = 1;
          }
      }
    }
  }

  vec_t in;
  for (int i = 0; i < 1083; ++i) {
    in.push_back(input[i]);
  }

  return nn->predict_label(in);
}

void printNN(const network<sequential> &nn) {
  for (int i = 0; i < nn.depth(); ++i) {
    if (nn[i]->layer_type() != "conv") continue;
    nn[i]->output_to_image().write("layers" + std::to_string(i) + ".png");
  }
}

int main() {
  network<sequential> nn;

  nn.load("nn");
  std::cout << "load nn" << std::endl;

  image<> img = nn.at<conv<leaky_relu>>(0).weight_to_image();
  img.write("filter0.png");

  DisplayBoard* board = new DisplayBoard();



  int nnBoard[19][19];
  while (true) {
    board->getBoard(nnBoard);

    Timer t;
    t.start();
    int index = nnPredict(&nn, nnBoard, !board->whoTurn());
    t.stop();
    t.print();

    printNN(nn);

    int row = index / 19,
        col = index % 19;
    std::cout << "Best: "
              << static_cast<char>(col + 65) << row + 1 << std::endl;

    bool validInput = false;

    while (!validInput) {
      // get user input
      board->getInput(&row, &col);

      // tries to play at (row, col)
      validInput = board->play(row, col);

      // handle invalid input
      if (!validInput)
        std::cout << "Invalid move\n";
    }
  }
}
