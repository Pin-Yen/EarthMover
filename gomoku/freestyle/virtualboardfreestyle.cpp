#include "virtualboardfreestyle.h"

#include "evaluatorfreestyle.h"

VirtualBoardFreeStyle* VirtualBoardFreeStyle::create() {
  EvaluatorFreeStyle::init();
  VirtualBoardFreeStyle* temp = new VirtualBoardFreeStyle();
  temp->init<VirtualBoardFreeStyle::EvaluatorFreeStyle>();
  return temp;
}

GameStatus VirtualBoardFreeStyle::play(int index) {
  return VirtualBoardGomoku::
      play<VirtualBoardFreeStyle::EvaluatorFreeStyle>(index);
}

void VirtualBoardFreeStyle::undo(int index) {
  VirtualBoardGomoku::undo<VirtualBoardFreeStyle::EvaluatorFreeStyle>(index);
}
