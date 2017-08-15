#include <fstream>
#include <iostream>

#include "../lib/json.h"

using json = nlohmann::json;

void ioMaker(const int board[15][15], bool blackTurn,
             const int output[15][15]);
int randomRotateMirror(int table[15][15]);
void rotateMirror(int table[15][15], int index);
void rotate(int table[15][15]);
void mirror(int table[15][15]);

std::ofstream outfile ("cnndata.txt");

int main() {
  srand((unsigned)time(NULL));

  std::ifstream i("jsondata.json");
  json j;
  i >> j;

  for (auto& game : j) {
    int board[15][15] = {0};
    int count = 0;
    for (auto& play : game["record"]) {
      if (play.is_null()) continue;

      ++count;

      int row = play["r"], col = play["c"];

      // Copy current board.
      int copyBoard[15][15];
      std::copy(&board[0][0], &board[0][0] + 225, &copyBoard[0][0]);

      int output[15][15] = {0};
      output[row][col] = 1;

      int r = randomRotateMirror(copyBoard);
      rotateMirror(output, r);
      ioMaker(copyBoard, (count & 1), output);

      // put chess in board (black: 1, white: -1)
      board[row][col] = (count & 1) ? 1 : -1;
    }
  }

  outfile.close();
}

void ioMaker(const int board[15][15], bool blackTurn,
             const int output[15][15]) {
  int input[4][15][15] = {0};

  // Input feature
  // Black
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (board[r][c] == 1) input[0][r][c] = 1;
    }
  }
  // White
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (board[r][c] == -1) input[1][r][c] = 1;
    }
  }
  // Empty
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (board[r][c] == 0) input[2][r][c] = 1;
    }
  }
  // BlackTurn
  for (int r = 0; r < 15; ++r) {
    for (int c = 0; c < 15; ++c) {
      if (blackTurn) input[3][r][c] = 1;
    }
  }

  static int count = 0;
  if (outfile.is_open()) {
    outfile << ++count << "\n";

    for (auto& feature : input) {
      for (int r = 0; r < 15; ++r) {
        for (int c = 0; c < 15; ++c) {
          outfile << feature[r][c] << " ";
        }
        outfile << "\n";
      }
      outfile << "\n";
    }
    for (int r = 0; r < 15; ++r) {
      for (int c = 0; c < 15; ++c) {
        outfile << output[r][c] << " ";
      }
      outfile << "\n";
    }
    outfile << "\n";
  } else  {
    std::cout << "Unable to open file" << std::endl;
  }
}

int randomRotateMirror(int table[15][15]) {
  int index = rand() % 8;
  for (int m = 0; m < 2; ++m) {
    for (int r = 0; r < 4; ++r) {
      if (index == m * 4 + r) return index;
      rotate(table);
    }
    mirror(table);
  }
}

void rotateMirror(int table[15][15], int index) {
  for (int m = 0; m < 2; ++m) {
    for (int r = 0; r < 4; ++r) {
      if (index == m * 4 + r) return;
      rotate(table);
    }
    mirror(table);
  }
}

void rotate(int table[15][15]) {
  int temp[15][15];
  // rotate 90 degrees clockwise (row -> col, col -> 14 - row)
  for (int r = 0; r < 15; ++r)
    for (int c = 0; c < 15; ++c)
      temp[c][14 - r] = table[r][c];

  for (int r = 0; r < 15; ++r)
    for (int c = 0; c < 15; ++c)
      table[r][c] = temp[r][c];
}

void mirror(int table[15][15]) {
  int temp[15][15];
  // mirror (row -> col, col -> row)
  for (int r = 0; r < 15; ++r)
    for (int c = 0; c < 15; ++c)
      temp[c][r] = table[r][c];

  for (int r = 0; r < 15; ++r)
    for (int c = 0; c < 15; ++c)
      table[r][c] = temp[r][c];
}
