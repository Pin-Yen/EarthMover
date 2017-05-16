#ifndef GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H
#define GOMOKU_RENJU_BASIC_VIRTUAL_BOARD_H

class VirtualBoardRenjuBasic : public VirtualBoardGomoku<8, VirtualBoard::Evaluator> {
  VirtualBoardRenjuBasic* clone() override {
    return new VirtualBoardRenjuBasic(this);
  }
};

#endif