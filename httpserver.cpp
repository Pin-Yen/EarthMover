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


HttpServer::HttpServer(AI *earthMover, DisplayBoard *board) {
  this->earthMover = earthMover;
  this->board = board;
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

void HttpServer::listenConnection() {
  listen(client, 1);

  socklen_t size = sizeof(serverAddress);

  while (true) {
    // accepts a new connection from the connection queue
    server = accept(client, (struct sockaddr*)&serverAddress, &size);
    if (server < 0) {
      std::cout <<"Failed to accept" << std::endl;
      return;
    }

    /* outputs client port number (only works for ipv4) */
    struct sockaddr_in *clientAddr = (struct sockaddr_in*) &serverAddress;
    int clientPort = ntohs(clientAddr->sin_port);

    std::cout << "client port: " << clientPort << std::endl;

    /* store request in buffer */
    const int REQUEST_BUFFER_SIZE = 4000;
    char requestBuffer[REQUEST_BUFFER_SIZE] = {'\0'};
    int recvReturnValue = recv(server, requestBuffer, REQUEST_BUFFER_SIZE, 0);
    std::cout << "request:\n" << requestBuffer <<std::flush;

    std::string request(requestBuffer);
    std::cout << "requestBufferLen:" << request.length() << std::endl;

    if (request.length() == 0) {
      responseHttpError(400);
      std::cout << "received empty request"<<std::endl;
      continue;
    }

    /* parse request directory */
    int directoryStart = request.find("/");
    int directoryEnd = request.find(" ", directoryStart);
    assert(directoryStart >= 0 &&  directoryEnd < 150);
    std::string directory = request.substr(directoryStart, directoryEnd - directoryStart );

    /* extract request body */
    std::string requestBody;
    int bodyStart = request.find("\r\n\r\n");
    if (bodyStart != std::string::npos) {
      bodyStart += 4;
      requestBody = request.substr(bodyStart);
    }

    if (directory == "/play") {
       /* extract row & col from encoded request body */
      int row = atoi(findAttributeInJson(requestBody, "row").c_str());
      int col = atoi(findAttributeInJson(requestBody, "col").c_str());

      requestAiPlay(row, col, false);

    } else if (directory == "/start") {

      isBlackAi = (findAttributeInJson(requestBody, "black") == "computer");
      isWhiteAi = (findAttributeInJson(requestBody, "white") == "computer");

      if (isBlackAi){
        requestAiPlay(-1, -1, true);
      } else {
        HttpResponse response(204);
        send(server, response.getHeaderString().c_str(), response.getHeaderString().length(), 0);
      }
    }
    else {

      /* redirect path */
      if (directory == "/")
        directory.append("board.html");
      else if (directory == "/favicon.ico")
        directory = std::string("/gomoku/src/chess_white.png");

      /* check permission and access resource */
      if (sanitize(directory)) {
        // if access to this directory is permitted
        std::ifstream resourceFile;
        resourceFile.open(directory.substr(1).c_str(), std::ios_base::in | std::ios_base::binary); /* use the sub-string from pos=1 to skip the first '/' in directory path */

        if( !resourceFile.is_open()) {
          responseHttpError(404);
        } else {
          std::cout << "200 ok" << std::endl;
          HttpResponse response(200);
          response.setContentTypeByFileExt(directory.substr(directory.find_last_of(".")));
          response.setBody(&resourceFile);
          std::string header = response.getHeaderString();
          std::cout << header << std::endl;
          // send header
          send(server, header.c_str(), header.length(), 0);
          // send body
          send(server, response.getBody(), response.getBodyLength(), 0);

          std::cout << "sent!" << std::endl;
        }

      } else {
        /* access denied */
        std::cout << "access denied" << std::endl;
        responseHttpError(403);
      }
    }

    close(server);
  }
}

void HttpServer::requestAiPlay(int clientRow, int clientCol, bool isFirstMove) {
  int gameStatus;

  /* play client's move if it is not the first move and not both players are ai */
  if (!isFirstMove && !(isBlackAi && isWhiteAi)) {
    /* stop background thread */
    earthMover->stopBGThread();

    /* play client's move */
    if( !board->play(clientRow, clientCol)) {
    /* invalid client play, either there's bug in client-side's chessboard script,
     * or the user is manually posting wrong data */

      responseHttpError(400);
      std::cout << "400 bad request" << std::endl;
      return;
    }
    gameStatus = earthMover->play(clientRow, clientCol, false);
    // TODO: handle win / lose
  }

  int EMRow, EMCol;
  earthMover->think(&EMRow, &EMCol);

  /* if at least one player is ai, play ai's move on the board */
  if (isBlackAi || isWhiteAi) {
    board->play(EMRow, EMCol);
    gameStatus = earthMover->play(EMRow, EMCol, true);
    // TODO: handle win / lose
  }

  /* return EM's play to client */
  HttpResponse response(200);
  response.setContentType("application/json")
          .addJson("row", EMRow)
          .addJson("col", EMCol);

  std::cout << response.getHeaderString();
  std::cout << response.getBody();
  send(server, response.getHeaderString().c_str(), response.getHeaderLength(), 0);
  send(server, response.getBody(), response.getBodyLength(), 0);
}


void HttpServer::responseHttpError(int errorCode) {
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