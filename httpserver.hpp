#ifndef HTTP_SERVER_
#define HTTP_SERVER_

#include "ai.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"

#include <string>
#include <thread>
#include <unordered_map>

class HttpServer
{
 public:
  HttpServer();

  ~HttpServer();

  // starts the HttpServer. (Listens and accepts connections.)
  void run();

  // Specifies the port to listen.
  static const int PORTS_[3];

 protected:
  // Read request from file discriptor `client` and returns a parsed HttpRequest object.
  HttpRequest readRequest(const int client);

  // Dispatch the HttpRequest to handlers.
  void dispatch(const int client, HttpRequest* request);

  // Check if the resource request is legal. returns true if legal, otherwise false.
  bool sanitize(std::string directory);

  // Sends an HTTP response back to the client.
  void sendResponse(const int client, HttpResponse *response);

  // Generates a random string.
  std::string sessionIdGenerator();

 private:

  void handlePlay(const int client, HttpRequest* request);
  void handleVisualize(const int client, HttpRequest* request);
  void handleResign(const int client, HttpRequest* request);
  void handlePass(const int client, HttpRequest* request);
  void handleUndo(const int client, HttpRequest* request);
  void handleThink(const int client, HttpRequest* request);
  void handleStart(const int client, HttpRequest* request);
  void handleResourceRequest(const int client, HttpRequest* request);


  // The limit of pending connections in the queue.
  static const int MAX_CONNECTION_QUEUE_ = 70;
  // The limit of EM instances.
  static const int MAX_EM_INSTANCE_ = 3;
  // Max characters to read in a single request
  static const int MAX_REQUEST_LENGTH_ = 1500;

  // session to id (session is the string of the http cookie, id is for indexing purposes)
  std::unordered_map<std::string, int> session2id_;

  // A list storing EarthMover instances.
  AI* emList_[MAX_EM_INSTANCE_];

  // Store threads. Each EM instance will only have at most 1 exclusive thread.
  // Index is the EM instance's id.
  std::thread threadList_[MAX_EM_INSTANCE_];

  // Controllers EMs' threads. `False` will stop the thread.
  bool emThreadController_[MAX_EM_INSTANCE_];

};

#endif