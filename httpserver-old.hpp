#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "ai.hpp"

#include <exception>
#include <fstream>
#include <netinet/in.h> /* sockaddr_in*/
#include <string>

class HttpServer {
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

  int client, server;
  struct sockaddr_in serverAddress;

  AI *earthMover;

  bool isBlackAi, isWhiteAi;

  /* Check if we should redirect the request uri. */
  void redirect(std::string *directory);

  /* Initializes a new game according to the settings specified in request.
   * Clear data of previous game if exist. */
  void handleStart(std::string requestBody);

  /* Plays a point specified in request. */
  bool handlePlay(std::string requestBody);

  /* request AI think */
  bool handleThink();

  /* undo */
  void handleUndo(std::string requestBody);

  /* pass */
  void handlePass();

  /* opponent resign, stop background thread */
  void handleResign();

  /* returns JSON Monte-Carlo tree to client */
  void handleVisualize();

  /* response a resource specified by "directory" to client, or some http error. */
  void handleResourceRequest(std::string requestBody, std::string directory);

  /* extracts directory from request */
  std::string parseRequestDirectory(std::string request);

  /* extracts body from request */
  std::string parseRequestBody(std::string request);

  /* Returns a error message to client
   * errorCode: http error code */
  void responseHttpError(int errorCode, const char* message);

  /* checks if the requested uri is permitted
   * returns false if not permitted */
  bool sanitize(std::string uri);

  /* given json data, find the value of 'attr' attribute and return string representation of the value */
  std::string findAttributeInJson(std::string json, const char* rawAttr);

};

class HttpResponse
{
 public:

  // Creates an empty HTTP response. Can optionally set the status code.
  HttpResponse(int httpResponseCode = -1);

  ~HttpResponse();

  // Sets the HTTP response status code.
  HttpResponse& setResponse(int statusCode) {this->statusCode = statusCode; }

  // Explicitly sets the content type.
  HttpResponse& setContentType(const char* type);

  // Sets content-type header according to fileExtension.
  HttpResponse& setContentTypeByFileExt(std::string fileExtension);

  // Populates the response body from the given file.
  HttpResponse& setBody(std::ifstream *file);

  // Popuates the response body from the given string.
  HttpResponse& setBody(std::string body);

  // Appends a cookie to the header.
  HttpResponse& addCookie(const char* name, const char* value);

  // Generates the header string, calcuates contentLength and headerLength.
  // After calling this, the state becomes STATE_COMPILED, which means this response is
  // ready to being served to the client. Further attemps to alter the response data are
  // invalid and will cause the `HTTPResponseException` be thrown.
  HttpResponse& compile();

  // ** The following methods will throw `HTTPResponseException` if the state is STATE_RAW

  std::string getHeaderString();

  int getHeaderLength() { return headerLength; }

  const char* getBody();

  int getBodyLength();

 private:
  static const unsigned char STATE_RAW = 0;
  static const unsigned char STATE_COMPILED = 1;

  // Given a status code, returns its description.
  static const char* status2String(int statusCode);

  std::string header;

  int statusCode;
  std::string contentType;
  std::string contentLength;
  std::string cookies;

  bool isBin;
  char *binBody;
  std::string body;

  int bodyLength;
  int headerLength;

  // recording the state of this response, should be STATE_RAW or STATE_COMPILED
  unsigned char state;

  class HttpResponseException : std::exception {
   public:
    HTTPResponseException(const char* description, int line, const char* file) {
      message.append(description).append("\n\tAt line ").append(std::to_string(line))
      .append(", in ").append(file);
    }

    virtual const char* what() const noexcept {
      return message.c_str()
    }

   private:
    std::string message;
  }
};

#endif