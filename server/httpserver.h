#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include <string>
#include <thread>
#include <unordered_map>

#include "../ai.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpServer {
 public:
  HttpServer(int port);

  ~HttpServer();

  // starts the HttpServer. (Listens and accepts connections.)
  void run();

 protected:
  // Read request from file discriptor `client`
  // and returns a parsed HttpRequest object.
  HttpRequest readRequest(const int client);

  // Dispatch the HttpRequest to handlers.
  void dispatch(const int client, HttpRequest* request);

  // Check if the resource request is legal.
  // Returns true if legal, otherwise false.
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
  void handleQuit(const int client, HttpRequest* request);
  void handleResourceRequest(const int client, HttpRequest* request);

  // The limit of pending connections in the queue.
  static const int MAX_CONNECTION_QUEUE_ = 70;
  // The limit of EM instances.
  static const int MAX_EM_INSTANCE_ = 3;
  // Max characters to read in a single request
  static const int MAX_REQUEST_LENGTH_ = 1500;

  // specifies which port to use.
  int port_;

  // session to id
  // (session is the string of the http cookie, id is for indexing purposes)
  std::unordered_map<std::string, int> session2instance_;

  // A list storing EarthMover instances.
  AI* emList_[MAX_EM_INSTANCE_];

  // Store threads. Each EM instance will only have at most 1 exclusive thread.
  // Index is the EM instance's id.
  std::thread threadList_[MAX_EM_INSTANCE_];

  // Controllers EMs' threads. `False` will stop the thread.
  bool emThreadController_[MAX_EM_INSTANCE_];
};

#endif  // HTTPSERVER_H_
