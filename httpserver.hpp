#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "ai.hpp"
#include "gomoku/displayboard.hpp"

#include <string>
#include <fstream>
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
  void listenConnection();

 private:
  const int PORT_NUMBER = 1202;
  class HttpResponse;

  int client, server;
  struct sockaddr_in serverAddress;

  // TODO: initialize earthmover
  AI *earthMover;
  DisplayBoard *board;

  /* request AI's play, and returns it to the client via http response */
  void requestAiPlay(int clientRow, int clientCol);

  /* Returns a error message to client
   * errorCode: http error code */
  void responseHttpError(int errorCode);

  /* checks if the requested uri is permitted
   * returns false if not permitted */
  bool sanitize(std::string uri);


};

class HttpServer::HttpResponse
{
 public:
  /* creates a httpResponse with statusCode = httpResponseCode*/
  HttpResponse(int httpResponseCode);

  /* set content-type header according to fileExtension */
  void setContentType(std::string fileExtension);

  /* sets the response body */
  void setBody(std::ifstream *file);

  /* returns a string of the request */
  std::string getResponseString();


 private:
  int statusCode;
  std::string contentType;
  std::string body;
  std::string status;
};

#endif