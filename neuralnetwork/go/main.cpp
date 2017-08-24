#include <assert.h>
#include <iostream>
#include <string>
#include <cstdlib>

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

int main() {
  network<sequential> nn;

  nn.load("nn");
  std::cout << "load nn" << std::endl;

  DisplayBoard* board = new DisplayBoard();

  int nnBoard[19][19];
  while (true) {
    board->getBoard(nnBoard);
    int index = nnPredict(&nn, nnBoard, !board->whoTurn());

    image<> img = nn[nn.depth() - 1]->output_to_image();
    img.write("output.png");

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
