#include "httpserver.hpp"

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

HttpServer::HttpServer() {
  client = socket(AF_INET, SOCK_STREAM, 0);

  if (client < 0)
    std::cout << "Error creating socket." << std::endl;

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htons(INADDR_ANY);
  serverAddress.sin_port = htons(PORT_NUMBER);

  if ((bind(client, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) < 0)
    std::cout << "Error binding connection, the socket has already been established...\n" << std::endl;

}

bool HttpServer::listenAndResponse() {
  listen(client, 1);

  socklen_t size = sizeof(serverAddress);
  server = accept(client, (struct sockaddr*)&serverAddress, &size);
  if (server < 0) {
    std::cout <<"Failed to accept" << std::endl;
    return false;
  }

  const int REQUEST_BUFFER_SIZE = 1000;
  char requestBuffer[REQUEST_BUFFER_SIZE] = {'\0'};
  recv(server, requestBuffer, REQUEST_BUFFER_SIZE, 0);
  std::cout << "request:\n" << requestBuffer;

  char webContentBuffer[BUFFER_SIZE];

  std::ifstream file;
  file.open(HTML_PAGE_PATH);

  /* If cannot open http file */
  if (!file.is_open()) {
    std::cout << "Failed to open html file." << std::endl;
    responseHttpError(500);
    return false;
  }

  /* read the file into memory, then send it to the client*/
  file.seekg(0, std::ios::end);
  int htmlContentLength = file.tellg();
  ++htmlContentLength; /* length = endposition + 1*/
  assert(htmlContentLength < BUFFER_SIZE);

  file.read(webContentBuffer, htmlContentLength);
  /* add string terminator at end of char[], doesn't matter in the http response,
   * but just in case we printed it out with std::cout when debugging */
  webContentBuffer[htmlContentLength] = '\0';

  send(server, webContentBuffer, htmlContentLength, 0);

  return true;
}

void HttpServer::responsePoint(int row, int col) {
  char response[50];

  sprintf(response, "{\"row\":%d,\"col\":%d}", row, col);
  send(server, response, strlen(response), 0);
}

void HttpServer::getClientPlay(int *row, int *col) {
  const int REQUEST_BUFFER_SIZE = 1000;
  char requestContent[REQUEST_BUFFER_SIZE] = {'\0'};
  recv(server, requestContent, REQUEST_BUFFER_SIZE, 0);

  std::string requestString(requestContent);

  /* extract row & col from encoded request body */
  int rowPosStart = requestString.find("row=") + 4;
  int rowPosEnd = requestString.find("&",rowPosStart);
  int rawRow = atoi(requestString.substr(rowPosStart, rowPosEnd - rowPosStart).c_str());

  int colPosStart = requestString.find("col=") + 4;
  int colPosEnd = requestString.find("\0",colPosStart);
  int rawCol = atoi(requestString.substr(colPosStart, colPosEnd - colPosStart).c_str());

  if (rawRow < 0 || rawRow >= 15 || rawCol < 0 || rawCol >= 15) {
    /* invalid input, either there's bug in client-side's chessboard script,
     * or the user is manually posting data */
    responseHttpError(400);
    *row = -1;
    *col = -1;
  }
  else {
    *row = rawRow;
    *col = rawCol;
  }
}


void HttpServer::responseHttpError(int errorCode) {
  char headerTemplate[] = "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\n";
  char header[100] = {'\0'};

  switch (errorCode) {
    case 500: sprintf(header, headerTemplate, errorCode, "INTERNAL SERVER ERROR"); break;
    case 400: sprintf(header, headerTemplate, errorCode, "BAD REQUEST");break;
    default: assert(0);
  }

  send(server, header, strlen(header), 0);
}