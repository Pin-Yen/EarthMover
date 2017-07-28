#include <iostream>

#include "httpserver.h"

int main() {
  srand((unsigned)time(NULL));

  HttpServer server;

  server.run();
}