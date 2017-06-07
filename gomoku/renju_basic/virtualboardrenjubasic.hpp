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

  inline int play(int row, int col) final override;
};

#include "evaluatorrenjubasic.hpp"

VirtualBoardRenjuBasic* VirtualBoardRenjuBasic::create() {
  EvaluatorRenjuBasic::init();
  VirtualBoardRenjuBasic *temp = new VirtualBoardRenjuBasic();
  temp->init<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>();
  return temp;
}

int VirtualBoardRenjuBasic::play(int row, int col) {
  return VirtualBoardGomoku::play<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>(row, col);
}


#endif