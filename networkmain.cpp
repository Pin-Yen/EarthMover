#include "ai.hpp"
#include "httpserver.hpp"

#include <iostream>

int main() {
  srand((unsigned)time(NULL));

  HttpServer server;

  server.listenConnection();
}