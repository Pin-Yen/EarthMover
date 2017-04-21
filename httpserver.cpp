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

    /* request handlers */
    if (directory == "/") {
      responeBoard();

    } else if(directory == "/play") {
       /* extract row & col from encoded request body */
      int rowPosStart = requestBody.find("row=") + 4;
      int rowPosEnd = requestBody.find("&",rowPosStart);
      int row = atoi(requestBody.substr(rowPosStart, rowPosEnd - rowPosStart).c_str());

      int colPosStart = requestBody.find("col=") + 4;
      int colPosEnd = requestBody.find("\0",colPosStart);
      int col = atoi(requestBody.substr(colPosStart, colPosEnd - colPosStart).c_str());

      if ( ! board->play(row, col)) {
        /* invalid input, either there's bug in client-side's chessboard script,
         * or the user is manually posting wrong data */
        responseHttpError(400);
      } else {
        requestAiPlay(row, col);
      }

    } else {
      /* unhandled directories */
      responseHttpError(404);
      // NOTE: be aware of directory-traversal-attack, do not pass unprocessed directory to file.open()
    }
  }
}

bool HttpServer::responseBoard() {

  std::ifstream file;
  file.open(HTML_PAGE_PATH);

  /* If cannot open http file */
  if (!file.is_open()) {
    std::cout << "Failed to open html file." << std::endl;
    responseHttpError(500);
    return false;
  }

  /* calculate file length */
  file.seekg(0, std::ios::end);
  int htmlContentLength = file.tellg();
  ++htmlContentLength; /* length = endposition + 1*/
  assert(htmlContentLength < BUFFER_SIZE);

  /* read the file into memory */
  char webContentBuffer[BUFFER_SIZE];
  file.read(webContentBuffer, htmlContentLength);

  /* add string terminator at end of char[], doesn't matter in the http response,
   * but just in case we printed it out with std::cout when debugging */
  webContentBuffer[htmlContentLength] = '\0';

  send(server, webContentBuffer, htmlContentLength, 0);

  return true;
}

void HttpServer::requestAiPlay(int clientRow, int clientCol) {
  int EMRow, EMCol;
  earthMover->clientPlay(clientRow,clientCol, &EMRow, &EMCol);
  // TODO: handle win / lose

  board->play(EMRow, EMCol);

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