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
  server = accept(client, (struct sockaddr*)&serverAddress, &size);
  if (server < 0) {
    std::cout <<"Failed to accept" << std::endl;
    return;
  }


  while (true) {
    /* store request in buffer */
    const int REQUEST_BUFFER_SIZE = 4000;
    char requestBuffer[REQUEST_BUFFER_SIZE] = {'\0'};
    recv(server, requestBuffer, REQUEST_BUFFER_SIZE, 0);
    std::cout << "recieving something" << std::flush;
    std::cout << "request:\n" << requestBuffer <<std::flush;

    std::string request(requestBuffer);
    std::cout << "requestBufferLen:" << request.length() << std::endl;
    /* chrome sends useless empty requests */
    if (request.length() == 0) {
      responseHttpError(400);
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

    std::cout << "path: " << directory << std::endl;
    /* if requested path is "/", redirect it to /gomoku/board.html */
    if (directory == "/") {
      directory.append("board.html");
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

    }
    // favicon
    if (directory == "/favicon.ico") {
      directory = std::string("/gomoku/src/chess_white.png");
    }

    /* requesting some resource */
    if (sanitize(directory)) {
        // if access to this directory is permitted
      std::ifstream resourceFile;
      resourceFile.open(directory.substr(1).c_str(), std::ios_base::in | std::ios_base::binary); /* use the sub-string from pos=1 to skip the first '/' in directory path */
      std::cout << "file path = " << directory.substr(1).c_str() << std::endl;

      if( !resourceFile.is_open()) {
        responseHttpError(404);
      } else {
        std::cout << "200 ok" << std::endl;
        HttpResponse response(200);
        response.setContentType(directory.substr(directory.find_last_of(".")));
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
  char responseBody[50];

  std::string response("HTTP/1.1 200 OK\r\n");
  response.append("Content-Type: application/json\r\n\r\n");

  sprintf(responseBody, "{\"row\":%d,\"col\":%d}", EMRow, EMCol);
  response.append(responseBody);

  send(server, response.c_str(), response.length(), 0);
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

void HttpServer::HttpResponse::setBody(std::ifstream *file) {
  file->seekg(0, std::ios_base::end);
  int fileLength = (int)file->tellg();
  bodyLength = fileLength;

  std::cout << "file length: "<<fileLength << std::endl;

  body = new char[fileLength];

  file->seekg(0, std::ios_base::beg);
  file->read(body, fileLength);

  /* set content-length header */
  char temp[30];
  sprintf(temp, "Content-Length: %d", fileLength);
  contentLength = std::string(temp);

}
std::string HttpServer::HttpResponse::getHeaderString(){
  std::string header;

  header.append(status).append("\r\n")
          .append(contentType).append("\r\n")
          .append(contentLength).append("\r\n")
          .append("\r\n");

  return header;
}

char* HttpServer::HttpResponse::getBody() {
  return body;
}

int HttpServer::HttpResponse::getBodyLength() {
  return bodyLength;
}

HttpServer::HttpResponse::~HttpResponse() {
  delete[] body;
}