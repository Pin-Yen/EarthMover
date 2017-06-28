#ifndef GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H
#define GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H
#include "../virtualboardgomoku.hpp"
class VirtualBoardRenjuBasic final : public VirtualBoardGomoku<10> {
 public:
  VirtualBoardRenjuBasic() : VirtualBoardGomoku() {}
  VirtualBoardRenjuBasic(VirtualBoardRenjuBasic* board) : VirtualBoardGomoku(board) {}

 private:
  class EvaluatorRenjuBasic;

  inline VirtualBoardRenjuBasic* create() final override;

  VirtualBoardRenjuBasic* clone() final override {
    return new VirtualBoardRenjuBasic(this);
  }

  inline int play(int index) final override;

  inline void undo(int index) final override;
};

#include "evaluatorrenjubasic.hpp"

VirtualBoardRenjuBasic* VirtualBoardRenjuBasic::create() {
  EvaluatorRenjuBasic::init();
  VirtualBoardRenjuBasic *temp = new VirtualBoardRenjuBasic();
  temp->init<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>();
  return temp;
}

int VirtualBoardRenjuBasic::play(int index) {
  return VirtualBoardGomoku::play<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>(index);
}

void VirtualBoardRenjuBasic::undo(int index) {
  VirtualBoardGomoku::undo<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>(index);
}

#endif