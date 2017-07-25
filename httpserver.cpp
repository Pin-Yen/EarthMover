#include "httpserver.hpp"

#include "lib/json.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using json = nlohmann::json;

HttpServer::HttpServer() {
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
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htons(INADDR_ANY);
  serverAddress.sin_port = htons(PORT_);

  if (bind(socketDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress) ) < 0)
    std::cerr << "failed to bind socket";

  // Listen to Connections.
  listen(socketDescriptor, MAX_CONNECTION_QUEUE_);
  socklen_t size = sizeof(serverAddress);

  while(true) {
    // Accepts a new connection.
    int client = accept(socketDescriptor, (struct sockaddr*)&serverAddress, &size);

    if (client < 0)
      std::cerr << "failed to accept\n";

    try {
      // Parse request.
      HttpRequest request = readRequest(client);
      printf("parsed request: path %s\n", request.path().c_str());
      // Dispatch parsed request to handlers.
      dispatch(client, &request);
    } catch (HttpResponse::HttpResponseException& e) {
      // Return bad request.
      // Just blame the client whatever the problem is XD.
      std::cerr << e.what() << "\n";
      HttpResponse response(400);
      response.compile();
      sendResponse(client, &response);
    }
  }
}

HttpRequest HttpServer::readRequest(const int client) {
  // Read data into buffer.
  char buffer[MAX_REQUEST_LENGTH_];
  int requestLength = read(client, buffer, MAX_REQUEST_LENGTH_);
  buffer[requestLength] = '\0';

  printf("%s\n", buffer);
  return HttpRequest(buffer);
}

void HttpServer::dispatch(const int client, HttpRequest* request) {
  //
  if (request->path() == "/play")
    handlePlay(client, request);
  else if(request->path() == "/visualize")
    handleVisualize(client, request);
  else if(request->path() == "/resign")
    handleResign(client, request);
  else if(request->path() == "/pass")
    handlePass(client, request);
  else if(request->path() == "/undo")
    handleUndo(client, request);
  else if(request->path() == "/think")
    handleThink(client, request);
  else if(request->path() == "/start")
    handleStart(client, request);
  else
    handleResourceRequest(client, request);

  return;
}

void HttpServer::sendResponse(const int client, HttpResponse* response) {
  printf("response:%s\n", response->getHeaderString().c_str());
  // Send header.
  send(client, response->getHeaderString().c_str(), response->getHeaderLength(), 0);
  // Send body.
  send(client, response->getBody(), response->getBodyLength(), 0);
  return;
}

void HttpServer::handlePlay(const int client, HttpRequest* request) {
  int id = session2id_[request->cookie("session")];

  // Stop EM's background thinking.
  emThreadController_[id] = false;

  // Wait till the background thinking thread has stopped.
  if (threadList_[id].joinable())
    threadList_[id].join();

  // Parse request body.
  json body(request->body());

  // Play and get winning status.
  int winning = emList_[id]->play(((int)body["row"]) * 15 + ((int)body["col"]));

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
  int id = session2id_[request->cookie("session")];

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
  int id = session2id_[request->cookie("session")];

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
  int id = session2id_[request->cookie("session")];

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
  int id = session2id_[request->cookie("session")];

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
  int id = session2id_[request->cookie("session")];
  AI* em = emList_[id];
  bool* controller = &(emThreadController_[id]);

  // Spin off the thinking process to a new thread, so that the server (main) thread wont be blocked.
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
    this->sendResponse(client, &response);

    // Return if game ends.
    if (whoWin != -1) {
      // TODO: Tell someone to remove EM instance, or, at least cut down the tree.
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
    instanceId = session2id_["session"];
  } catch (std::out_of_range e){
    // Generate new cookie.
    sessionId = sessionIdGenerator();

    // Create new em instance.
    for (int i = 0; i < MAX_EM_INSTANCE_; ++i) {
      if (emList_[i] == NULL) {
        emList_[i] = new AI();
        instanceId = i;
      }
    }

    // Map sessionId to instance id if successfully created instance.
    // Else return 503
    if (instanceId != -1)
      session2id_.insert({sessionId, instanceId});
    else {
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
  emList_[instanceId]->reset(body["level"], body["rule"]);

  // Response
  HttpResponse response(204);
  response.addCookie("session", sessionId.c_str())
    .compile();
  sendResponse(client, &response);

  return;
}

void HttpServer::handleResourceRequest(const int client, HttpRequest* request) {
  // Return 403 forbiddin if illegel.
  if (!sanitize(request->path())) {
    HttpResponse response(403);
    response.compile();
    sendResponse(client, &response);
    return;
  }

  // Open file.
  std::ifstream resourceFile;
  resourceFile.open(request->path().substr(1).c_str(), std::ios_base::in | std::ios_base::binary);

  // If can't open file, return 404.
  if(!resourceFile.is_open()) {
    HttpResponse response(404);
    response.compile();
    sendResponse(client, &response);
    return;
  }

    HttpResponse response(200);
    response.setContentTypeByFileExt(request->path().substr(request->path().find_last_of(".")))
      .setBody(&resourceFile)
      .compile();

    sendResponse(client, &response);

  return;
}

bool HttpServer::sanitize(std::string directory) {
  if (directory == "/index.html") return true;

  // check if the directory is under /gomoku/src/
  std::string allowedRoot("/gomoku/src/");
  if(directory.find(allowedRoot) != 0)
    return false;

  // check if the directory contains ..
  if(directory.find("..") != std::string::npos)
    return false;

  return true;
}

std::string HttpServer::sessionIdGenerator() {
  char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  std::string sessionId;
  for (int i = 0; i < 10; ++i)
    sessionId.push_back(charset[rand() % (sizeof(charset) - 1)]);

  return sessionId;
}