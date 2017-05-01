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
  HttpServer();

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

  bool isBlackAi, isWhiteAi;

  /* Check if we should redirect the request uri. */
  void redirect(std::string *directory);

  /* Initializes a new game according to the settings specified in request.
   * Clear data of previous game if exist. */
  void handleStart(std::string requestBody);

  /* Plays a point specified in request. */
  bool handlePlay(std::string requestBody);

  /* response a resource specified by "directory" to client, or some http error. */
  void handleResourceRequest(std::string requestBody, std::string directory);

  /* extracts directory from request */
  std::string parseRequestDirectory(std::string request);

  /* extracts body from request */
  std::string parseRequestBody(std::string request);


  /* Request AI's play, and returns it to the client via http response.
   * If requesting AI for the first move, clientRow & clientCol should be -1, and isFirstMove should be true.
   * Returns true if someone won. */
  bool requestAiPlay(int clientRow, int clientCol, bool isFirstMove);

  /* Returns a error message to client
   * errorCode: http error code */
  void responseHttpError(int errorCode, const char* message);

  /* checks if the requested uri is permitted
   * returns false if not permitted */
  bool sanitize(std::string uri);

  /* given json data, find the value of 'attr' attribute and return string representation of the value */
  std::string findAttributeInJson(std::string json, const char* rawAttr);

};

class HttpServer::HttpResponse
{
 public:
  /* creates a httpResponse with statusCode = httpResponseCode*/
  HttpResponse(int httpResponseCode);

  ~HttpResponse();

  /* set content-type header according to fileExtension */
  void setContentTypeByFileExt(std::string fileExtension);

  /* explicitly set content type */
  HttpResponse& setContentType(const char* type);

  /* Add JSON {attribute:value} to response body, returns *this for chainning */
  HttpResponse& addJson(std::string attribute, int value);

  HttpResponse& addJson(std::string attribute, const char* value);

  /* sets the response body */
  void setBody(std::ifstream *file);

  std::string getHeaderString();

  int getHeaderLength() { return headerLength; }

  const char* getBody();

  int getBodyLength();

 private:


  int statusCode;
  std::string contentType;
  std::string contentLength;
  char *binBody;
  std::string body;
  bool isBin;
  int bodyLength;
  int headerLength;

  std::string status;
};

#endif