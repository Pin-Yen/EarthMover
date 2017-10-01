#include "virtualboardrenjubasic.h"

#include "evaluatorrenjubasic.h"

VirtualBoardRenjuBasic* VirtualBoardRenjuBasic::create() {
  EvaluatorRenjuBasic::init();
  VirtualBoardRenjuBasic *temp = new VirtualBoardRenjuBasic();
  temp->init<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>();
  return temp;
}

GameStatus VirtualBoardRenjuBasic::play(int index) {
  return VirtualBoardGomoku::
      play<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>(index);
}

void VirtualBoardRenjuBasic::undo(int index) {
  VirtualBoardGomoku::undo<VirtualBoardRenjuBasic::EvaluatorRenjuBasic>(index);
}