#include <iostream>

#include "server/httpserver.h"
#include <cstdlib>

int main(int argc, char** argv) {
  srand((unsigned)time(NULL));
  if (argc < 2) {
    std::cout << "Please specify port number." << std::endl;
    return 1;
  }
  HttpServer server(atoi(argv[1]));

  server.run();
  return 0;
}
