#include "httpserver.hpp"
#include "ai.hpp"

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

HttpServer::HttpServer(AI *earthMover, Displayboard *board) {
  this->earthMover = earthMover;
  this->board = board;

  client = socket(AF_INET, SOCK_STREAM, 0);

  if (client < 0)
    std::cout << "Error creating socket." << std::endl;

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htons(INADDR_ANY);
  serverAddress.sin_port = htons(PORT_NUMBER);

  if ((bind(client, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) < 0)
    std::cout << "Error binding connection, the socket has already been established...\n" << std::endl;

}

void listen() {
  listen(client, 1);

  socklen_t size = sizeof(serverAddress);
  server = accept(client, (struct sockaddr*)&serverAddress, &size);
  if (server < 0) {
    std::cout <<"Failed to accept" << std::endl;
    return false;
  }


  while (true) {
    /* store request in buffer */
    const int REQUEST_BUFFER_SIZE = 1000;
    char requestBuffer[REQUEST_BUFFER_SIZE] = {'\0'};
    recv(server, requestBuffer, REQUEST_BUFFER_SIZE, 0);
    std::cout << "request:\n" << requestBuffer;

    std::string request(requestBuffer);

    /* parse request directory */
    int directoryStart = request.find("/");
    int directoryEnd = request.find(" ", directoryStart);
    std::string directory = request.substr(directoryStart, directoryEnd - directoryStart + 1);

    /* extract request body */
    int bodyStart = (int)request.find("\r\n\r\n") + 4;
    std::string requestBody = request.substr(bodyStart);

    /* if requested path is "/", redirect it to /gomoku/board.html */
    if (directory == "/") {
      directory.append("gomoku/board.html");
    }

    /* request handlers */
    if(directory == "/play") {
      /* extract row & col from encoded request body */
      int rowPosStart = requestBody.find("row=") + 4;
      int rowPosEnd = requestBody.find("&",rowPosStart);
      int row = atoi(requestBody.substr(rowPosStart, rowPosEnd - rowPosStart).c_str());

      int colPosStart = requestBody.find("col=") + 4;
      int colPosEnd = requestBody.find("\0",colPosStart);
      int col = atoi(requestBody.substr(colPosStart, colPosEnd - colPosStart).c_str());

      requestAiPlay(row, col);

    } else {
      /* requesting some resource */
      if (sanitize(directory)) {
        // if access to this directory is permitted
        fstream resourceFile;
        resourceFile.open(directory.substr(1).c_str()); /* use the sub-string from pos=1 to skip the first '/' in directory path */

        if( !resource.is_open()) {
          responseHttpError(404);
        } else {
          HttpResponse response(200);
          response.setContentType(directory.substr(directory.find_last_of(".")));
          response.setBody(resourceFile);
          std::string responseData = response.getResponseString();
          send(server, responseData.c_str(), responseData.length(), 0);
        }

      }

      /* access denied */
      responseHttpError(403);

    }
  }
}

void HttpServer::requestAiPlay(int clientRow, int clientCol) {
  /* play client's move */
  if( !board->play(clientRow, clientCol)) {
    /* invalid client play, either there's bug in client-side's chessboard script,
     * or the user is manually posting wrong data */

    responseHttpError(400);
    return;
  }
  int gameStatus = earthMover->play(clientRow, clientCol);
  // TODO: handle win / lose


  int EMRow, EMCol;
  earthMover->think(clientRow,clientCol, &EMRow, &EMCol);

  board->play(EMRow, EMCol);
  gameStatus = earthMover->play(EMRow, EMCol);
  // TODO: handle win / lose

  /* return EM's play to client */
  char response[50];


  sprintf(response, "{\"row\":%d,\"col\":%d}", row, col);
  send(server, response, strlen(response), 0);
}


void HttpServer::responseHttpError(int errorCode) {
  char headerTemplate[] = "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\n";
  char header[100] = {'\0'};

  switch (errorCode) {
    case 400: sprintf(header, headerTemplate, errorCode, "BAD REQUEST"); break;
    case 404: sprintf(header, headerTemplate, errorCode, "NOT FOUND"); break;
    case 500: sprintf(header, headerTemplate, errorCode, "INTERNAL SERVER ERROR"); break;
    default: assert(0);
  }

  send(server, header, strlen(header), 0);
}

HttpServer::HttpResponse::HttpResponse(int httpResponseCode) {
  statusCode = httpResponseCode;
  status.append("HTTP/1.1 ");
  status.append(std::to_string(statusCode));
  status.append(" ");

  switch (statusCode) {
    case 200: status.append("OK"); break;
  }
}

void HttpServer::HttpResponse::setContentType(std::string fileExtension) {
  contentType.append("Content-Type: ")
  if (fileExtension.strcmp(".png"))
    contentType.append("image/png");
  else if (fileExtension.strcmp(".js"))
    contentType.append("application/javascript");
  else if (fileExtension.strcmp(".html"))
    contentType.append("text/html");

}

void HttpServer::HttpResponse::setBody(File *file) {
  file->seek(ios_base::end);
  int fileLength = (int)file->tellg() + 1;
  /* max size for a source file is 5mb */
  const int MAX_SRC_FILE_SIZE = 5 * 1024 * 1024;
  assert (fileLength <= MAX_SRC_FILE_SIZE);

  char buffer[MAX_SRC_FILE_SIZE];
  file->read(buffer, fileLength);

  body = string(buffer);
}

std::string HttpServer::HttpResponse::getResponseString() {
  std::string response();

  response.append(status).append("\r\n")
          .append(contentType).append("\r\n")
          .append("\r\n")
          .append(body);

  return response;
}