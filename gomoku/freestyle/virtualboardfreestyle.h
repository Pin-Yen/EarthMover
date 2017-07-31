#ifndef GOMOKU_FREESTYLE_VIRTUALBOARDFREESTYLE_H_
#define GOMOKU_FREESTYLE_VIRTUALBOARDFREESTYLE_H_

#include "../virtualboardgomoku.h"

class VirtualBoardFreeStyle final : public VirtualBoardGomoku<8> {
 public:
  VirtualBoardFreeStyle() : VirtualBoardGomoku() {}
  VirtualBoardFreeStyle(
      const VirtualBoardFreeStyle& board) : VirtualBoardGomoku(board) {}

 private:
  class EvaluatorFreeStyle;

  inline VirtualBoardFreeStyle* create() final;

  VirtualBoardFreeStyle* clone() final {
    return new VirtualBoardFreeStyle(*this);
  }

  inline int play(int index) final;

  inline void undo(int index) final;
};

#include "evaluatorfreestyle.h"

VirtualBoardFreeStyle* VirtualBoardFreeStyle::create() {
  EvaluatorFreeStyle::init();
  VirtualBoardFreeStyle* temp = new VirtualBoardFreeStyle();
  temp->init<VirtualBoardFreeStyle::EvaluatorFreeStyle>();
  return temp;
}

int VirtualBoardFreeStyle::play(int index) {
  return VirtualBoardGomoku::
      play<VirtualBoardFreeStyle::EvaluatorFreeStyle>(index);
}

void VirtualBoardFreeStyle::undo(int index) {
  VirtualBoardGomoku::undo<VirtualBoardFreeStyle::EvaluatorFreeStyle>(index);
}

#endif  // GOMOKU_FREESTYLE_VIRTUALBOARDFREESTYLE_H_
