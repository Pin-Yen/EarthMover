#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

#include "server/httpserver.h"

int main(int argc, char** argv) {
  srand((unsigned)time(NULL));
  if (argc < 2) {
    std::cout << "Please specify port number." << std::endl;
    return 1;
  }
  HttpServer server(atoi(argv[1]));

  // print pid for debugging
  std::cout << "pid: " << getpid() << "\n";

  server.run();
  return 0;
}
