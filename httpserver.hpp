#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_


#include <netinet/in.h> /* sockaddr_in*/

class HttpServer
{

 public:
  /* constructor, initialize serverAddress and establishes socket connection */
  HttpServer();
  ~HttpServer();

  /* listens for client connection, and process the request */
  void listen();

 private:
  const int PORT_NUMBER = 1202;

  int client, server;
  struct sockaddr_in serverAddress;

  // TODO: initialize earthmover
  AI *earthMover;

  /* transmit chessboard html to client,
   * returns true on success */
  bool responseBoard();

  /* request AI's play, and returns it to the client via http response */
  void requestAiPlay(int clientRow, int clientCol);

  /* Returns a error message to client
   * errorCode: http error code */
  void responseHttpError(int errorCode);

};

#endif