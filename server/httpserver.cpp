#include "httpserver.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <cerrno>

#include "../lib/json.h"

using json = nlohmann::json;

HttpServer::HttpServer(int port) {
  port_ = port;
  for (int i = 0; i < MAX_EM_INSTANCE_; ++i) {
    emList_[i] = NULL;
    emThreadController_[i] = NULL;
  }
}

HttpServer::~HttpServer() {
  for (int i = 0; i < MAX_EM_INSTANCE_; ++i) {
    if (emList_[i] != NULL)
      delete emList_[i];
  }
}

void HttpServer::run() {
  int socketDescriptor = -1;

  // Open a socket, and obtain its descriptor.
  socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDescriptor == -1)
    std::cerr << "Failed to open socket.\n";

  // Bind socket to address.
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htons(INADDR_ANY);
  serverAddress.sin_port = htons(port_);

  if (bind(socketDescriptor, reinterpret_cast<sockaddr*>(&serverAddress),
             sizeof(serverAddress)) == 0) {
      std::cerr << "Listening on port " << port_ << "\n" << std::flush;
  } else {
    std::cerr << "Port " << port_ << " not available, please use another port\n" << std::flush;
  }

  // Listen to Connections.
  listen(socketDescriptor, MAX_CONNECTION_QUEUE_);
  socklen_t size = sizeof(serverAddress);

  while (true) {
    // Accepts a new connection.
    int client = accept(socketDescriptor,
                        reinterpret_cast<sockaddr*>(&serverAddress), &size);
    if (client < 0)
      std::cerr << "failed to accept\n";

    try {
      // Parse request.
      HttpRequest request = readRequest(client);
      std::cerr << "request path: " << request.path() << std::flush;

      // Dispatch parsed request to handlers.
      dispatch(client, &request);
    } catch(const HttpRequest::EmptyRequestException e){
      // close the connection if recieved empty request.
      std::cerr << "empty request => ignored\n";
      if (close(client) != 0)
        std::cerr << "Failed to close socket after receiving empty response\n";

    } catch (const HttpResponse::HttpResponseException e) {
      // Return bad request.
      // Just blame the client whatever the problem is XD.
      std::cerr << e.what() << "\n";
      HttpResponse response(400);
      response.compile();
      sendResponse(client, &response);
    } catch (const HttpRequest::BadRequestException e) {
      std::cerr << e.what();
      HttpResponse response(400);
      response.compile();
      sendResponse(client, &response);
    }
  }
}

HttpRequest HttpServer::readRequest(const int client) {
  // Read data into buffer.
  std::string rawRequest;
  char buffer[MAX_REQUEST_LENGTH_ + 10];
  int totalBytesRead = 0, newReadBytes = 0;

  // Read until the whole header is read.
  do {
    newReadBytes = recv(client, buffer, MAX_REQUEST_LENGTH_, 0);
    totalBytesRead += newReadBytes;
    buffer[newReadBytes] = '\0';
    rawRequest += buffer;
  } while (rawRequest.find("\r\n\r\n") == std::string::npos && newReadBytes != 0);

  if (totalBytesRead == 0)
    throw HttpRequest::EmptyRequestException("Empty request.");


  HttpRequest request(rawRequest);

  int bodyStartPos = rawRequest.find("\r\n\r\n") + 4;
  std::string body = rawRequest.substr(bodyStartPos);

  while(body.length() < request.contentLength()) {
    newReadBytes = recv(client, buffer, MAX_REQUEST_LENGTH_, 0);
    totalBytesRead += newReadBytes;
    buffer[newReadBytes] = '\0';
    body += buffer;
  }

  // Add body to request
  request.addBody(body);


  return request;
}

void HttpServer::dispatch(const int client, HttpRequest* request) {
  //
  if (request->path() == "/play")
    handlePlay(client, request);
  else if (request->path() == "/visualize")
    handleVisualize(client, request);
  else if (request->path() == "/resign")
    handleResign(client, request);
  else if (request->path() == "/pass")
    handlePass(client, request);
  else if (request->path() == "/undo")
    handleUndo(client, request);
  else if (request->path() == "/think")
    handleThink(client, request);
  else if (request->path() == "/start")
    handleStart(client, request);
  else
    handleResourceRequest(client, request);

  return;
}

void HttpServer::sendResponse(const int client, HttpResponse* response) {
  // Send header.
  int wholeLen = response->getHeaderLength() + response->getBodyLength();
  char buffer[wholeLen];

  response->getWhole(buffer);
  send(client, buffer, wholeLen, 0);

  // Wait for client's FIN signal.
  char *useless[10];
  if (recv(client, useless, 10, 0) != 0) {
    std::cerr << "Not receiving client's FIN signal\n";
    std::cerr << strerror(errno);
  }

  if (close(client) != 0) {
    std::cerr << "Failed to close connection\n";
    std::cerr << strerror(errno);
  }
  std::cerr << " => "<< response->statusCode() << "\n" << std::flush;
  return;
}

void HttpServer::handlePlay(const int client, HttpRequest* request) {
  int id = session2instance_.at(request->cookie("session"));

  // Stop EM's background thinking.
  emThreadController_[id] = false;

  // Wait till the background thinking thread has stopped.
  if (threadList_[id].joinable())
    threadList_[id].join();

  // Parse request body.
  json body = json::parse(request->body());

  // Play and get winning status.
  int winning = emList_[id]->play(static_cast<int>(body["row"]) * 15 +
                                  static_cast<int>(body["col"]));

  json responseData;
  responseData["winner"] = winning;

  // Prepare a response.
  HttpResponse response(200);
  response.setContentType("application/json")
      .setBody(responseData.dump())
      .compile();

  // Send response to client.
  sendResponse(client, &response);

  return;
}

void HttpServer::handleVisualize(const int client, HttpRequest* request) {
  int id = -1;

  try {
    id = session2instance_.at(request->cookie("session"));
  } catch (std::out_of_range& e) {
    HttpResponse response(200);
    response.setContentType("application/json")
        .setBody("")
        .compile();

    sendResponse(client, &response);

    return;
  }

  // Prepare response.
  HttpResponse response(200);
  response.setContentType("application/json")
      .setBody(emList_[id]->getTreeJSON())
      .compile();

  // Send response to client.
  sendResponse(client, &response);

  return;
}

void HttpServer::handleResign(const int client, HttpRequest* request) {
  int id = session2instance_.at(request->cookie("session"));

  // Stop background thinking thread, if exists.
  emThreadController_[id] = false;
  if (threadList_[id].joinable())
    threadList_[id].join();

  emList_[id]->resign();

  HttpResponse response(204);
  response.compile();

  sendResponse(client, &response);

  // TODO: delete EM instance.
  return;
}

void HttpServer::handlePass(const int client, HttpRequest* request) {
  int id = session2instance_.at(request->cookie("session"));

  // Stop EM's background thinking.
  emThreadController_[id] = false;
  // Wait till the background thinking thread has stopped.
  if (threadList_[id].joinable())
    threadList_[id].join();

  emList_[id]->pass();

  HttpResponse response(204);
  response.compile();

  sendResponse(client, &response);

  return;
}

void HttpServer::handleUndo(const int client, HttpRequest* request) {
  int id = session2instance_.at(request->cookie("session"));

  // Stop EM's background thinking.
  emThreadController_[id] = false;
  // Wait till the background thinking thread has stopped.
  if (threadList_[id].joinable())
    threadList_[id].join();

  // Parse request body.
  json body = json::parse(request->body());

  // Extract how many times to undo.
  int times = body["times"];

  emList_[id]->undo(times);

  HttpResponse response(204);
  response.compile();

  sendResponse(client, &response);

  return;
}

void HttpServer::handleThink(const int client, HttpRequest* request) {
  int id = session2instance_.at(request->cookie("session"));

  AI* em = emList_[id];
  bool* controller = &(emThreadController_[id]);

  // Stop previous background thinking threads, if exists
  if (threadList_[id].joinable()) {
    *controller = false;
    threadList_[id].join();
  }

  *controller = true;

  // Spin off the thinking process to a new thread,
  // so that the server (main) thread wont be blocked.
  threadList_[id] = std::thread([this, em, controller, client, request]() {
    // Earth Mover's decision, encoded in a single integer (row * 15 + col).
    int index = em->think();

    int whoWin = -1;

    // TODO: interate pass() and play()
    if (index == -1)
      em->pass();
    else
      whoWin = em->play(index);

    // Prepare response.
    json body;
    body["row"] = (index == -1 ? -1 : index / 15);
    body["col"] = (index == -1 ? -1 : index % 15);
    body["winner"] = whoWin;

    HttpResponse response(200);
    response.setContentType("application/json")
        .setBody(body.dump())
        .compile();

    // Send response;
    sendResponse(client, &response);

    // Return if game ends.
    if (whoWin != -1) {
      // TODO: Tell someone to remove EM instance,
      //       or, at least cut down the tree.
      return;
    }

    // Think in background.
    em->thinkInBackground(controller);
  });
}

void HttpServer::handleStart(const int client, HttpRequest* request) {
  // Check if the user already have an existing EM instance.
  // If not, create an new instance and set a cookie.
  std::string sessionId = request->cookie("session");

  int instanceId = -1;

  try {
    instanceId = session2instance_.at(sessionId);
  } catch (std::out_of_range& e) {
    // Generate new cookie.
    sessionId = sessionIdGenerator();

    // Create new em instance.
    for (int i = 0; i < MAX_EM_INSTANCE_; ++i) {
      if (emList_[i] == NULL) {
        emList_[i] = new AI();
        instanceId = i;
        break;
      }
    }

    // Map sessionId to instance id if successfully created instance.
    // Else return 503
    if (instanceId != -1) {
      session2instance_.insert({sessionId, instanceId});
    } else {
      HttpResponse response(503);
      response.compile();
      sendResponse(client, &response);
      return;
    }
  }

  // Stop backgronud thinking thread, if exists.
  emThreadController_[instanceId] = false;
  if (threadList_[instanceId].joinable())
    threadList_[instanceId].join();

  // Reset EM.
  json body = json::parse(request->body());
  emList_[instanceId]->reset(static_cast<int>(body["level"]),
                             static_cast<int>(body["rule"]));

  // Response
  HttpResponse response(204);
  response.addCookie("session", sessionId.c_str()).compile();
  sendResponse(client, &response);


  return;
}

void HttpServer::handleQuit(const int client, HttpRequest* request) {

}

void HttpServer::handleResourceRequest(const int client,
                                       HttpRequest* request) {
  // Return 403 forbiddin if illegel.
  if (!sanitize(request->path())) {
    HttpResponse response(403);
    response.compile();
    sendResponse(client, &response);
    return;
  }

  // Open file.
  std::ifstream resourceFile;
  resourceFile.open(request->path().substr(1).c_str(),
                    std::ios_base::in | std::ios_base::binary);

  // If can't open file, return 404.
  if (!resourceFile.is_open()) {
    HttpResponse response(404);
    response.compile();
    sendResponse(client, &response);
    return;
  }

    HttpResponse response(200);
    response.setContentTypeByFileExt(request->path().substr(request->path()
                                                    .find_last_of(".")))
            .setBody(&resourceFile)
            .compile();

    sendResponse(client, &response);

    resourceFile.close();

  return;
}

bool HttpServer::sanitize(std::string directory) {
  if (directory == "/index.html") return true;

  // check if the directory is under /gomoku/src/
  std::string allowedRoot("/gomoku/src/");
  if (directory.find(allowedRoot) != 0)
    return false;

  // check if the directory contains ..
  if (directory.find("..") != std::string::npos)
    return false;

  return true;
}

std::string HttpServer::sessionIdGenerator() {
  char charset[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::string sessionId;
  for (int i = 0; i < 10; ++i)
    sessionId.push_back(charset[rand() % (sizeof(charset) - 1)]);

  return sessionId;
}
