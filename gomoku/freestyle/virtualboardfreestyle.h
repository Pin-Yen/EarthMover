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

  VirtualBoardFreeStyle* create() final;

  VirtualBoardFreeStyle* clone() final {
    return new VirtualBoardFreeStyle(*this);
  }

  GameStatus play(int index) final;

  void undo(int index) final;
};

#endif  // GOMOKU_FREESTYLE_VIRTUALBOARDFREESTYLE_H_
