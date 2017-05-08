#include "httpserver.hpp"
#include "ai.hpp"
#include "gomoku/displayboard.hpp"

#include <iostream>

int main() {
  srand((unsigned)time(NULL));

  char mode;

  while (true){
    std::cout << "Please type N for server mode or L for local mode (N/L)\n";
    std::cin >> mode;

    if (mode == 'N' || mode == 'L') break;

    std::cout << "Invalid input. ";
  }


  if (mode == 'N') {
    HttpServer server;

    server.listenConnection();

  } else if (mode == 'L') {
    int row, col;

    DisplayBoard board;
    AI earthMover(10000);

    while (true) {
      earthMover.think(&row, &col);

      board.getInput(&row, &col);
      board.play(row, col);

      if (earthMover.play(row, col, true)) {
        /* someone wins */
        std::cout << (earthMover.whoTurn() ? "white wins" : "black wins");
        break;
      }
    }
  }
}