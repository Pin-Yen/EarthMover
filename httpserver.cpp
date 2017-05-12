#include "httpserver.hpp"
#include "ai.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define HTML_PAGE_PATH "test.html"
#define BUFFER_SIZE 10000
#define DEBUG_NETWORK

HttpServer::HttpServer() {
  board = new DisplayBoard();
  earthMover = new AI(1);
  client = -1;
  server = -1;

  client = socket(AF_INET, SOCK_STREAM, 0);

  if (client < 0)
    std::cout << "Error creating socket." << std::endl;

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htons(INADDR_ANY);
  serverAddress.sin_port = htons(PORT_NUMBER);

  if ((bind(client, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) < 0)
    std::cout << "Error binding connection, the socket has already been established...\n" << std::endl;

}

HttpServer::~HttpServer() {
  delete earthMover;
  delete board;
}

void HttpServer::listenConnection() {
  listen(client, 1);

  socklen_t size = sizeof(serverAddress);

  // server loop
  while (true) {
    bool stopGame = false;
    // init game
    //game loop
    while (!stopGame) {
      // accept connection, and read request.
      server = accept(client, (struct sockaddr*)&serverAddress, &size);
      if (server < 0) {
        std::cout <<"Failed to accept" << std::endl;
        continue;
      }

      /* outputs client port number (only works for ipv4) */
      struct sockaddr_in *clientAddr = (struct sockaddr_in*) &serverAddress;
      int clientPort = ntohs(clientAddr->sin_port);
      #ifdef DEBUG_NETWORK
      std::cout << "client port: " << clientPort << std::endl;
      #endif

      /* read request into buffer */
      const int REQUEST_BUFFER_SIZE = 4000;
      char requestBuffer[REQUEST_BUFFER_SIZE] = {'\0'};
      int recvReturnValue = recv(server, requestBuffer, REQUEST_BUFFER_SIZE, 0);
      #ifdef DEBUG_NETWORK
      std::cout << "request:\n" << requestBuffer <<std::flush;
      #endif

      std::string request(requestBuffer);

      // check if it is a empty request
      if (request.length() == 0) {
        responseHttpError(400, "Empty request");
        continue;
      }

      /* parse request directory */
      std::string directory = parseRequestDirectory(request);
      std::string requestBody = parseRequestBody(request);

      // redirect url
      redirect(&directory);

      // handle requests according to their directory path
      if (directory == "/start") {
        handleStart(requestBody);
      } else if(directory == "/play") {
        stopGame = handlePlay(requestBody);
      } else {
        handleResourceRequest(requestBody, directory);
      }
      close(server);
    }
  }
}

void HttpServer::redirect(std::string *directory) {

  if (*directory == "/") {
    directory->append("board.html");
  } else if (*directory == "/favicon.ico") {
    // favicon, should replace with EM icon.
    *directory = std::string("/gomoku/src/chess_white.png");
  }
}

void HttpServer::handleStart(std::string requestBody) {
  // isBlackAi = (findAttributeInJson(requestBody, "black") == "computer");
  // isWhiteAi = (findAttributeInJson(requestBody, "white") == "computer");
  // future game settings go here (strength, rule, etc)

  earthMover->reset();

  HttpResponse response(204);
  send(server, response.getHeaderString().c_str(), response.getHeaderString().length(), 0);
}

bool HttpServer::handlePlay(std::string requestBody) {
  /* extract row & col from encoded request body */
  int userRow = atoi(findAttributeInJson(requestBody, "row").c_str());
  int userCol = atoi(findAttributeInJson(requestBody, "col").c_str());
  bool shouldAiThink = (findAttributeInJson(requestBody, "think") == "true");


  bool isWinning = false; // true: winning, false: not winning.
  bool winnerColor; //true: black, false: white.

  // If Ai should play the move specified in the request.
  if (userRow != -1 && userCol != -1) {
    // third param: Think in background if not requested to play by the client.
    isWinning = earthMover->play(userRow, userCol, !shouldAiThink);
  }

  int AiRow, AiCol;

  // EM think & play if user's move is not winning & EM's play is requested by the client.
  if (shouldAiThink & !isWinning) {
    earthMover->think(&AiRow, &AiCol);
    isWinning = earthMover->play(AiRow, AiCol, true);
  }

  // Fill in the response data
  HttpResponse response(200);
  response.setContentType("application/json")
          .addJson("row", AiRow)
          .addJson("col", AiCol);

  if (isWinning)
    response.addJson("winner", (earthMover->whoTurn())? "white" : "black");
  else
    response.addJson("winner", "none");

  std::cout << response.getHeaderString();
  std::cout << response.getBody();

  // Sent response
  send(server, response.getHeaderString().c_str(), response.getHeaderLength(), 0);
  send(server, response.getBody(), response.getBodyLength(), 0);

  // Returns true to stop gameloop if someone is winning.
  return isWinning;

}

void HttpServer::handleResourceRequest(std::string requestBody, std::string directory) {

  if (! sanitize(directory)) {
    responseHttpError(403, "Requesting resource from forbidden uri");
  } else {

      // if access to this directory is permitted
    std::ifstream resourceFile;
      /* use the sub-string from pos=1 to skip the first '/' in directory path */
    resourceFile.open(directory.substr(1).c_str(), std::ios_base::in | std::ios_base::binary);

    if( !resourceFile.is_open()) {
      responseHttpError(404, "can't open resource file, does the file exist?");
    } else {
      HttpResponse response(200);
      response.setContentTypeByFileExt(directory.substr(directory.find_last_of(".")));
      response.setBody(&resourceFile);
      std::string header = response.getHeaderString();
        #ifdef DEBUG_NETWORK
      std::cout << header << std::endl;
        #endif
        // send header
      send(server, header.c_str(), header.length(), 0);
        // send body
      send(server, response.getBody(), response.getBodyLength(), 0);
        #ifdef DEBUG_NETWORK
      std::cout << "sent!" << std::endl;
        #endif
    }

  }
}

std::string HttpServer::parseRequestDirectory(std::string request) {
  int directoryStart = request.find("/");
  int directoryEnd = request.find(" ", directoryStart);
  assert(directoryStart >= 0 &&  directoryEnd < 150);

  return request.substr(directoryStart, directoryEnd - directoryStart );
}

std::string HttpServer::parseRequestBody(std::string request) {
  int bodyStart = request.find("\r\n\r\n");
  if (bodyStart != std::string::npos)
    bodyStart += 4;

  return request.substr(bodyStart);
}

void HttpServer::responseHttpError(int errorCode, const char *description = "") {
  char headerTemplate[] = "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\n";
  char header[100] = {'\0'};

  switch (errorCode) {
    case 400: sprintf(header, headerTemplate, errorCode, "BAD REQUEST"); break;
    case 403: sprintf(header, headerTemplate, errorCode, "FORBIDDEN"); break;
    case 404: sprintf(header, headerTemplate, errorCode, "NOT FOUND"); break;
    case 500: sprintf(header, headerTemplate, errorCode, "INTERNAL SERVER ERROR"); break;
    default: assert(0);
  }

  send(server, header, strlen(header), 0);

  #ifdef DEBUG_NETWORK
  std::cout << errorCode << " error **" << description << "**" << std::endl;
  #endif
}

bool HttpServer::sanitize(std::string uri) {
  if (uri == "/board.html") return true;
  /* check if the uri is under /gomoku/src/ */
  std::string allowedRoot("/gomoku/src/");
  if(uri.find(allowedRoot) != 0)
    return false;

    /* check if the uri contains .. */
  if(uri.find("..") != std::string::npos)
    return false;

  return true;
}

std::string HttpServer::findAttributeInJson(std::string json, const char* rawAttr) {
  std::string attr(rawAttr);

  int startPos = json.find("\"" + attr + "\":") + attr.length() + 3;
  int endPos = std::min(json.find(",", startPos), json.find("}", startPos));

  assert(endPos != std::string::npos && startPos != std::string::npos);
  std::string value = json.substr(startPos, endPos - startPos);

  /* if the value is string type, erase its qoutation marks */
  if (value.front() == '\"' && value.back() == '\"') {
    value.erase(value.length() - 1, 1);
    value.erase(0, 1);
  }
  return value;
}

HttpServer::HttpResponse::HttpResponse(int httpResponseCode) {
  binBody = NULL;
  isBin = false;

  statusCode = httpResponseCode;
  status.append("HTTP/1.1 ");
  status.append(std::to_string(statusCode));
  status.append(" ");

  switch (statusCode) {
    case 200: status.append("OK"); break;
    case 204: status.append("NO CONTENT"); break;
  }
}

HttpServer::HttpResponse& HttpServer::HttpResponse::setContentType(const char* type) {
  contentType.append("Content-Type: ")
  .append(type);

  return *this;
}

void HttpServer::HttpResponse::setContentTypeByFileExt(std::string fileExtension) {
  contentType.append("Content-Type: ");

  if (fileExtension.compare(".png") == 0)
    contentType.append("image/png");
  else if (fileExtension.compare(".js") == 0)
    contentType.append("application/javascript");
  else if (fileExtension.compare(".html") == 0)
    contentType.append("text/html");
  else if(fileExtension.compare(".css") == 0)
    contentType.append("text/css");
  else
    std::cout << "unrecognized file type" << std::endl;

}

HttpServer::HttpResponse& HttpServer::HttpResponse::addJson(std::string attribute, int value) {
  attribute.insert(0, "\"");
  attribute.append("\":");

  char valueChar[12];
  sprintf(valueChar, "%d", value);
  attribute.append(valueChar);

  int insertPos = body.find('}');

  if (insertPos == std::string::npos) {
    body.append("{}");
    insertPos = 1;
  } else {
    body.insert(insertPos, ",");
    ++insertPos;
  }

  body.insert(insertPos, attribute);
  bodyLength = body.length();

  return *this;
}

HttpServer::HttpResponse& HttpServer::HttpResponse::addJson(std::string attribute, const char* value) {
  attribute.insert(0, "\"");
  attribute.append("\":");

  std::string valueString(value);

  valueString.insert(0, "\"");
  valueString.append("\"");
  attribute.append(valueString);

  int insertPos = body.find('}');

  if (insertPos == std::string::npos) {
    body.append("{}");
    insertPos = 1;
  } else {
    body.insert(insertPos, ",");
    ++insertPos;
  }

  body.insert(insertPos, attribute);
  bodyLength = body.length();

  return *this;
}

void HttpServer::HttpResponse::setBody(std::ifstream *file) {
  isBin = true;

  file->seekg(0, std::ios_base::end);
  int fileLength = (int)file->tellg();
  bodyLength = fileLength;

  std::cout << "file length: "<<fileLength << std::endl;

  if (binBody != NULL) {
    delete binBody;
  }
  binBody = new char[fileLength];

  file->seekg(0, std::ios_base::beg);
  file->read(binBody, fileLength);

}

std::string HttpServer::HttpResponse::getHeaderString(){
  /* set content-length header */
  char temp[30];
  sprintf(temp, "Content-Length: %d", bodyLength);
  contentLength = std::string(temp);

  std::string header;

  header.append(status).append("\r\n");

  if (contentType != "")
    header.append(contentType).append("\r\n");

  header.append(contentLength).append("\r\n")
  .append("Connection: closed").append("\r\n")
  .append("\r\n");

  headerLength = header.length();
  return header;
}

const char* HttpServer::HttpResponse::getBody() {
  if (binBody)
    return binBody;
  else
    return body.c_str();
}

int HttpServer::HttpResponse::getBodyLength() {
  return bodyLength;
}

HttpServer::HttpResponse::~HttpResponse() {
  if (binBody != NULL)
    delete[] binBody;
}