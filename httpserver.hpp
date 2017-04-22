#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_


#include <netinet/in.h> /* sockaddr_in*/

class HttpServer
{

 public:
  /* constructor
   * initialize serverAddress and establishes socket connection
   * set AI reference */
  HttpServer(AI *earthMover, DisplayBoard *board);
  ~HttpServer();

  /* listens for client connection, and process the request */
  void listen();

 private:
  const int PORT_NUMBER = 1202;

  int client, server;
  struct sockaddr_in serverAddress;

  // TODO: initialize earthmover
  AI *earthMover;
  DisplayBoard *board;

  /* transmit chessboard html to client,
   * returns true on success */
  bool responseBoard();

  /* request AI's play, and returns it to the client via http response */
  void requestAiPlay(int clientRow, int clientCol);

  /* Returns a error message to client
   * errorCode: http error code */
  void responseHttpError(int errorCode);

  /* checks if the requested uri is permitted
   * returns false if not permitted */
  bool sanitize(std::string uri) {

    /* check if the uri is under /gomoku/src/ */
    std::string allowedRoot("/gomoku/src/");
    if(uri.find(allowedRoot) != 0)
      return false;

    /* check if the uri contains .. */
    if(uri.find("..") != string::npos)
      return false;

    return true;
  }

};

#endif