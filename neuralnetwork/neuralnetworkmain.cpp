#include <assert.h>
#include <iostream>
#include <string>
#include <cstdlib>

#include "../gomoku/displayboard.cpp"

#include "../lib/tiny-dnn/tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;

void nnPredict(network<sequential> *nn,
               int board[15][15], bool blackTurn, int* row, int* col) {
  int input[900] = {0};

  // Input feature
  // Black
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (board[r][c] == 1) input[r * 15 + c] = 1;
    }
  }
  // White
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (board[r][c] == -1) input[225 + r * 15 + c] = 1;
    }
  }
  // Empty
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (board[r][c] == 0) input[450 + r * 15 + c] = 1;
    }
  }
  // BlackTurn
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (blackTurn) input[675 + r * 15 + c] = 1;
    }
  }

  vec_t result = nn->predict(input);
  double maxVal = 0.0;
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (result[r * 15 + c] > maxVal) {
        maxVal = result[r * 15 + c];
        *row = r;
        *col = c;
      }
    }
  }
}

int main() {
  network<sequential> nn;

  nn.load("nn");
  std::cout << "load nn" << std::endl;

  DisplayBoard* board = new DisplayBoard();
  int nnBoard[15][15] = {0};
  while (true) {
    int row, col;

    bool whoTurn = board->whoTurn();

    nnPredict(&nn, nnBoard, !whoTurn, &row, &col);
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
    nnBoard[row][col] = whoTurn ? -1 : 1;
  }
}
