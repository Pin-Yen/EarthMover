#ifndef GOMOKU_FREESTYLE_VIRTUAL_BOARD_H
#define GOMOKU_FREESTYLE_VIRTUAL_BOARD_H

class VirtualBoardFreeStyle : public VirtualBoardGomoku<8> {
 public:
  VirtualBoardFreeStyle() : VirtualBoardGomoku() {}
  VirtualBoardFreeStyle(VirtualBoardFreeStyle* board) : VirtualBoardGomoku(board) {}

  VirtualBoardFreeStyle* create();

  VirtualBoardFreeStyle* clone() override {
    return new VirtualBoardFreeStyle(this);
  }

  int play(int row, int col) override;
 private:
  class EvaluatorFreeStyle;
};

#include "evaluatorfreestyle.hpp"

VirtualBoardFreeStyle* VirtualBoardFreeStyle::create() {
  VirtualBoardFreeStyle* temp = new VirtualBoardFreeStyle();
  temp->init<VirtualBoardFreeStyle::EvaluatorFreeStyle>();
  return temp;
}

int VirtualBoardFreeStyle::play(int row, int col) {
  VirtualBoardGomoku::play<VirtualBoardFreeStyle::EvaluatorFreeStyle>(row, col);
}

#endif