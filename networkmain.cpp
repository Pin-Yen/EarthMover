#include <iostream>

#include "server/httpserver.h"

int main() {
  srand((unsigned)time(NULL));

  HttpServer server;

  server.run();
}
