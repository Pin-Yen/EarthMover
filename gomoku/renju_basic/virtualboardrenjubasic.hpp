#ifndef GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H
#define GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H

class VirtualBoardRenjuBasic : public VirtualBoardGomoku<10> {
  VirtualBoardRenjuBasic* clone() override {
    return new VirtualBoardRenjuBasic(this);
  }
 protected:
  class EvaluatorRenjuBasic : public VirtualBoard::Evaluator;
};

#endif