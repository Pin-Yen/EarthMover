#include "httpserver.hpp"
#include "ai.hpp"
#include "gomoku/displayboard.hpp"

#include <iostream>

int main() {
  srand((unsigned)time(NULL));

  HttpServer server;

  server.listenConnection();
}