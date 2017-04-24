#include "httpserver.hpp"
#include "ai.hpp"
#include "gomoku/displayboard.hpp"

#include <iostream>

int main() {
  char mode;

  while (true){
    std::cout << "Please type N for server mode or L for local mode (N/L)\n";
    std::cin >> mode;

    if(mode != 'N' && mode !='L')
      std::cout << "Invalid input. ";
    else
      break;
  }

  DisplayBoard board;
  AI earthMover(10000, &board);

  if (mode == 'N') {
    HttpServer server(&earthMover, &board);

    // TODO: determine who's black and who's white

    server.listenConnection();

  } else if (mode == 'L') {
    int inputRow = -1, inputCol = -1;
    int EMRow , EMCol;
    bool gameEnd = false;

    while (! gameEnd){
      earthMover.think(inputRow, inputCol, &EMRow, &EMCol);

      board.getInput(&inputRow, &inputCol);
      board.play(inputRow, inputCol);

      if (earthMover.play(inputRow, inputCol)) {
        /* someone wins */
        gameEnd = true;

        if (earthMover.getWhoTurn())
          std::cout << "black wins";
        else
          std::cout << "white wins";
      }
    }
  }
}