#ifndef GOMOKU_FREESTYLE_VIRTUAL_BOARD_H
#define GOMOKU_FREESTYLE_VIRTUAL_BOARD_H

class VirtualBoardFreeStyle : public VirtualBoardGomoku<8> {
 public:
  VirtualBoardFreeStyle() : VirtualBoardGomoku() {}
  VirtualBoardFreeStyle(VirtualBoardFreeStyle* board) : VirtualBoardGomoku(board) {}

  VirtualBoardFreeStyle* clone() override {
    return new VirtualBoardFreeStyle(this);
  }

 protected:
  class EvaluatorFreeStyle : public VirtualBoardGomoku::Evaluator;
};

#endif