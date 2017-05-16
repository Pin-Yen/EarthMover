#ifndef GOMOKU_FREESTYLE_VIRTUAL_BOARD_H
#define GOMOKU_FREESTYLE_VIRTUAL_BOARD_H

class VirtualBoardFreeStyle : public VirtualBoardGomoku<8, VirtualBoard::Evaluator> {
  VirtualBoardFreeStyle(VirtualBoardFreeStyle* board) : VirtualBoardGomoku(board) {}

  VirtualBoardFreeStyle* clone() override {
    return new VirtualBoardFreeStyle(this);
  }
};

#endif