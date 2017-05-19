#ifndef GOMOKU_FREESTYLE_VIRTUAL_BOARD_H
#define GOMOKU_FREESTYLE_VIRTUAL_BOARD_H

class VirtualBoardFreeStyle : public VirtualBoardGomoku<8> {
 public:
  VirtualBoardFreeStyle() : VirtualBoardGomoku() {}
  VirtualBoardFreeStyle(VirtualBoardFreeStyle* board) : VirtualBoardGomoku(board) {}

  VirtualBoardFreeStyle* create() {
    VirtualBoardFreeStyle* temp = new VirtualBoardFreeStyle();
    temp->init();
    return temp;
  }

  VirtualBoardFreeStyle* clone() override {
    return new VirtualBoardFreeStyle(this);
  }
};

#endif