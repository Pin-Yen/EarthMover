#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

using namespace std;

int main(){
  int server, client;
  const int PORT_NUMBER = 8080;
  struct sockaddr_in serverAddress;
  socklen_t size;

  client = socket(AF_INET, SOCK_STREAM, 0);

  if (client < 0) {
    std::cout << "error creating socket\n";
    return -1;
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htons(INADDR_ANY);
  serverAddress.sin_port = htons(PORT_NUMBER);
  if ((bind(client, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) < 0)
  {
    cout << "=> Error binding connection, the socket has already been established..." << endl;
    return -1;
  }

  listen(client, 1);

  size = sizeof(serverAddress);

  server = accept(client, (struct sockaddr*)&serverAddress, &size);

  if (server < 0) {
    cout << "failed to accept\n";
    return -1;
  }

  const int BUFFER_SIZE = 1024, MESSAGE_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  recv(server, buffer, BUFFER_SIZE, 0);
  cout << buffer << endl;

  fstream file;
  char message[MESSAGE_SIZE];
  file.open("test.html", ios::in);

  if (!file) {
    cout << "file not found\n";
    return -2;
  }

  file.read(message, sizeof(message));
  cout << message << endl;
  file.close();

  send(server, message, strlen(message), 0);

  return 0;
}