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

  VirtualBoardRenjuBasic* create() final;

  VirtualBoardRenjuBasic* clone() final {
    return new VirtualBoardRenjuBasic(*this);
  }

  GameStatus play(int index) final;

  void undo(int index) final;
};

#endif  // GOMOKU_RENJU_BASIC_VIRTUALBOARDRENJUBASIC_H_
