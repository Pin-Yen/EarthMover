#ifndef GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H
#define GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H
#include "../virtualboardgomoku.hpp"
class VirtualBoardRenjuBasic final : public VirtualBoardGomoku<10> {
 public:
  VirtualBoardRenjuBasic() : VirtualBoardGomoku() {}
  VirtualBoardRenjuBasic(VirtualBoardRenjuBasic* board) : VirtualBoardGomoku(board) {}

  inline VirtualBoardRenjuBasic* create() final override;

  VirtualBoardRenjuBasic* clone() final override {
    return new VirtualBoardRenjuBasic(this);
  }

 protected:
  inline int play(int row, int col) final override;
 private:
  class EvaluatorRenjuBasic;
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