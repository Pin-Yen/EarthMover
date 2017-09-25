#ifndef GAMETREE_H_
#define GAMETREE_H_

#include <string>
#include <utility>

#include "const.h"
#include "lib/json.h"
#include "memorypool.h"
#include "virtualboard.h"

class GameTree {
 public:
  GameTree();
  ~GameTree();

  // Initialize GameTree.
  void init(VirtualBoard* board);

  // Monty Carlo Tree Search.
  // Keep searching until reach 'cycle'.
  // Return false if the search ends prematurely.
  bool mcts(int cycle);

  // Monty Carlo Tree Search.
  // Stop if the point that count most times > 'minCount',
  // will check it after each batch.
  void mcts(int batch, int minCount);

  // Monty Carlo Tree Search.
  // Keep searching until 'thinking' becomes false,
  void mcts(const bool* continueThinking);
  // or reach the 'maxCycle'.
  void mcts(int maxCycle, const bool* thinking);

  // Monty Carlo Tree Search.
  // Line 26's multi-thread version.
  void mcts(int threadAmount, int batch, int minCount);
  // Line 32's multi-thread version.
  void mcts(int threadAmount, int maxCycle, const bool* thinking);

  // get the child that has highest playout from current node
  int mctsResult() const;

  // Called when a REAL point is played,
  // updates the current node and return game status.
  GameStatus play(int index);

  // pass
  void pass();

  // undo
  void undo();

  // Returns the whole tree in JSON format
  std::string getTreeJSON();

  // getter
  VirtualBoard* getCurrentBoard() { return currentBoard_; }

 private:
  class Node;

  class RAVE {
   public:
    RAVE() : count_(0), winLoseCount_(0) {}

    void update(SearchStatus result) {
      ++count_;
      if (result == WIN) ++winLoseCount_;
      if (result == LOSE) --winLoseCount_;
    }

    double winRate() const {
      return (count_ + winLoseCount_) / (count_ * 2.0);
    }

    void minus(const RAVE* rave) {
      count_ -= rave->count_;
      winLoseCount_ -= rave->winLoseCount_;
    }

    void merge(const RAVE* rave) {
      count_ += rave->count_;
      winLoseCount_ += rave->winLoseCount_;
    }

   private:
    int count_, winLoseCount_;
  };

  RAVE* rave_;

  // MCTS function.
  // Keep select the child node until reach a leaf or a winning node.
  // Return status and selectde node.
  std::pair<SearchStatus, Node*> selection(VirtualBoard* board);

  // MCTS function.
  // Simulate the game and return status.
  SearchStatus simulation(VirtualBoard* board) const;

  // MCTS function
  // Back propagation form leaf to current node.
  void backProp(Node* node, SearchStatus result, bool whoTurn);

  // Copy all children in 'srcNode' to 'destNode'.
  void copyAllChildren(const Node* srcNode, Node* destNode);

  // Copy tree, new tree's root is the current node of 'source'.
  void copy(const GameTree* source);

  // Merge all tree into this.
  void mergeTree(GameTree* tree);

  // Merge all children in 'mergedNode' into 'mergingNode'
  void mergeAllChildren(Node* mergingNode, const Node* mergedNode);

  void minusTree(GameTree* beMinusTree, const GameTree* minusTree);

  void minusAllChildren(Node* beMinusNode, const Node* minusNode);

  // returns the part of the tree below 'node' in JSON format
  nlohmann::json getSubTreeJSON(Node *node, bool whoTurn);

  Node *root_, *currentNode_;

  VirtualBoard* currentBoard_;

  MemoryPool pool_;
};

#endif  // GAMETREE_H_
