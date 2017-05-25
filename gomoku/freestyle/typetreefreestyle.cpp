#include "../chesstype.hpp"
#include "../status.hpp"
#include "../../virtualboard.hpp"
#include "../virtualboardgomoku.hpp"
#include "virtualboardfreestyle.hpp"
#include "../evaluator.hpp"
#include "typetreefreestyle.hpp"

#include <iostream>
#include <iomanip>

#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

bool VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle::isInit = false;

/* initialize root*/
typename VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle::Node*
  VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle::root = new Node();


void VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle::plantTree() {

  // create tree seed
  STATUS status[analyze_length];
  for (int i = 0; i < analyze_length; ++i)
    status[i] = EMPTY;

  // grow tree
  dfs(root, status, analyze_length / 2, -1, false, false);
}


/* Depth First Search
 * parameters of the initial call should be:
 * location: length / 2, move = -1, connect = 0 */

/* connect is used to prevent already exist five while length == 11
 * for example : OOOOO*OOX-- ; --X  *OOOOO
 *               ^^^^^               ^^^^^ */
void VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle::dfs(Node *root, STATUS *status, int location,
                                            int move, bool blackBlock, bool whiteBlock) {
  /* if status == black or white, set block == true*/
  switch (status[location]) {
    case BLACK:
      blackBlock = true; break;
    case WHITE:
      whiteBlock = true;
  }

  if ((blackBlock && whiteBlock) || status[location] == BOUND ||
      location <= 0 || location >= analyze_length - 1) {
    if (move == 1) {
      /* reached leaf */

      /* set type */
      root->type[0] = typeAnalyze(status, BLACK, true);
      root->type[1] = typeAnalyze(status, WHITE, true);

      /* set child node to NULL*/
      for (int i = 0; i < 4; ++i)
        root->childNode[i] = NULL;

      return;
    } else {
      /* set this node to jump node */
      root->jump = true;

      /* jump to middle of the status */
      move += 2;
      location = analyze_length / 2;

      /* reset block */
      blackBlock = false; whiteBlock = false;
    }
  }

  /* move location */
  location += move;

  root->type[0] = NULL; root->type[1] = NULL;

  const STATUS s[4] = {BLACK, WHITE, EMPTY, BOUND};

  for (int i = 0; i < 4; ++i) {
    root->childNode[i] = new Node();
    status[location] = s[i];
    dfs(root->childNode[i], status, location, move, blackBlock, whiteBlock);
  }

  /* restore current location to EMPTY
   * note: without this line, the classification process should still work fine,
   * but will result in printing out garbage values on EMPTY points */
  status[location] = EMPTY;
}

