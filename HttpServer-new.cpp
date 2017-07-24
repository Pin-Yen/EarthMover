#include "httprequest.hpp"
#include "lib/json.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <exception>

using json = nlohmann::json;

void HttpServer::run() {
  int socketDescriptor = -1;

  // Open a socket, and obtain its descriptor.
  socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDescriptor == -1)
    std::cerr << "Failed to open socket.\n";

  // Bind socket to address.
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htons(INADDR_ANY);
  serverAddress.sin_port = htons(PORT);

  if (bind(socketDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress) ) < 0)
    std::cerr << "failed to bind socket";

  // Listen to Connections.
  listen(socketDescriptor, MAX_CONNECTION_QUEUE);
  socklen_t size = sizeof(serverAddress);

  while(true) {
    // Accepts a new connection.
    int client = accept(socketDescriptor, (struct sockaddr*)&serverAddress, &size);

    if (client < 0)
      std::cerr << "failed to accept\n";

    // Parse request.
    HttpRequest request;
    try {
      request = readRequest(client);
    } catch (std::exception e) {
      // Return bad request.
      HttpResponse response(400);
      response.compile();
      sendResponse(&response);
      continue;
    }

    dispatch(cleint, request);
  }
}

HttpRequest HttpServer::readRequest(const int client) {
  // Read data into buffer.
  char buffer[MAX_REQUEST_LENGTH_];
  read(client, buffer, MAX_REQUEST_LENGTH_);

  return HttpRequest(buffer);
}

void HttpServer::dispatch(const int client, HttpRequest* request) {
  //
  if (request->path() == 'play')
    handlePlay(client, request)
}

void HttpServer::sendResponse(const int client, Response* response) {
  // Send header.
  send(client, response.getHeaderString().c_str(), response.getHeaderLength(), 0);
  // Send body.
  send(cleint, response.getBody(), response.getBodyLength(), 0);
  return;
}

void HttpServer::handlePlay(const int client, HttpRequest* request) {
  int id = session2id(request->cookie("session"));

  // Stop EM's background thinking.
  emThreadController_[id] = false;

  // Wait till the background thinking thread has stopped.
  emList_[id]->thread.join();

  // Parse request body.
  json body(request->body());

  // Play and get winning status.
  int winning = emList_[id]->play(body["row"], body["col"]);

  json responseData;
  responseData["winner"] = winning;

  // Prepare a response.
  HttpResponse response(200);
  response.setContentType("application/json")
    .setBody(responseData.dump())
    .compile();

  // Send response to client.
  sendResponse(client, &response);

  // TODO: delete EM instance if game ends.

  return;
}

void HttpServer::handleVisualize(const int client, HttpRequest* request) {
  int id = session2id(request->cookie("session"));

  // Prepare response.
  HttpResponse response(200);
  response.setContentType("application/json")
    .setBody(emList_[id]->getTreeJSON());
    .compile();

  // Send response to client.
  sendResponse(client, &response);

  return;
}

void HttpServer::handleResign(const int client, HttpRequest request) {
  int id = session2id(request->cookie("session"));

  emList_[id]->resign();

  HttpResponse response(204);
  response.compile();

  send(client, &response);

  // TODO: delete EM instance.
  return;
}

void HttpServer::handlePass(const int client, HttpRequest request) {
  int id = session2id(request->cookie("session"));

  emList_[id]->pass();

  HttpResponse response(204);
  response.compile();

  send(client, &response);

  return;
}
