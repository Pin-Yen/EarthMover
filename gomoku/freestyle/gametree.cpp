#include "gametree.hpp"

GameTree::GameTree()
{
  currentNode = (Node*)malloc(sizeof(Node));
  currentNode->board = new VirtualBoard();
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      currentNode->childNode[r][c] = NULL;
}

void GameTree::play(int row, int col)
{
  currentNode->childNode[row][col] = (Node*)malloc(sizeof(Node));
  currentNode = currentNode->childNode[row][col];
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      currentNode->childNode[r][c] = NULL;
}