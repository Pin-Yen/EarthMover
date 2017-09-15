#ifndef GOMOKU_RENJU_BASIC_VIRTUALBOARDRENJUBASIC_H_
#define GOMOKU_RENJU_BASIC_VIRTUALBOARDRENJUBASIC_H_

#include "../virtualboardgomoku.h"

class VirtualBoardRenjuBasic final : public VirtualBoardGomoku<10> {
 public:
  VirtualBoardRenjuBasic() : VirtualBoardGomoku() {}
  VirtualBoardRenjuBasic(
      const VirtualBoardRenjuBasic& board) : VirtualBoardGomoku(board) {}

 private:
  class EvaluatorRenjuBasic;

  inline VirtualBoardRenjuBasic* create() final;

  VirtualBoardRenjuBasic* clone() final {
    return new VirtualBoardRenjuBasic(*this);
  }

  inline GameStatus play(int index) final;

  inline void undo(int index) final;
};

#include "evaluatorrenjubasic.h"

VirtualBoardRenjuBasic* VirtualBoardRenjuBasic::create() {
  EvaluatorRenjuBasic::init();
  VirtualBoardRenjuBasic *temp = new VirtualBoardRenjuBasic();
  temp->init<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>();
  return temp;
}

GameStatus VirtualBoardRenjuBasic::play(int index) {
  return VirtualBoardGomoku::
      play<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>(index);
}

void VirtualBoardRenjuBasic::undo(int index) {
  VirtualBoardGomoku::undo<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>(index);
}

#endif  // GOMOKU_RENJU_BASIC_VIRTUALBOARDRENJUBASIC_H_
