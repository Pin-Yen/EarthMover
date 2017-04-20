#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

class HttpServer
{

 public:
  /* constructor, initialize serverAddress and establishes socket connection */
  HttpServer();
  ~HttpServer();

  /* sends EM's decision to the client via a http response */
  void responsePoint(int rol, int col);

  /* listens for client connection, and transmit the initial webpage to the client via a http response */
  void listenAndResponse();

  /* listen for client post requests and return the client's play in the provided pointers */
  void getClientPlay(int* row, int* col);

 private:
  const int PORT_NUMBER = 1202;
  int client, server;
  struct sockaddr_in serverAddress;

  /* Prepares the initial webpage content.
   * Returns a char* pointing to the start of the content array
   * called by listenAndResponse */
  char* prepareWebpage();

};

#endif