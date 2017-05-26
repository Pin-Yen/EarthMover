#ifndef GOMOKU_FREESTYLE_VIRTUAL_BOARD_H
#define GOMOKU_FREESTYLE_VIRTUAL_BOARD_H

class VirtualBoardFreeStyle final : public VirtualBoardGomoku<8> {
 public:
  VirtualBoardFreeStyle() : VirtualBoardGomoku() {}
  VirtualBoardFreeStyle(VirtualBoardFreeStyle* board) : VirtualBoardGomoku(board) {}

  inline VirtualBoardFreeStyle* create() final override;

  VirtualBoardFreeStyle* clone() final override {
    return new VirtualBoardFreeStyle(this);
  }

 protected:
  inline int play(int row, int col) final override;
 private:
  class EvaluatorFreeStyle;
};

#include "evaluatorfreestyle.hpp"

VirtualBoardFreeStyle* VirtualBoardFreeStyle::create() {
  EvaluatorFreeStyle::init();
  VirtualBoardFreeStyle* temp = new VirtualBoardFreeStyle();
  temp->init<VirtualBoardFreeStyle::EvaluatorFreeStyle>();
  return temp;
}

int VirtualBoardFreeStyle::play(int row, int col) {
  return VirtualBoardGomoku::play<VirtualBoardFreeStyle::EvaluatorFreeStyle>(row, col);
}

#endif