#include "httprequest.hpp"
#include <string>

HttpRequest::HttpRequest(const char* rawRequest) {
  std::string request(rawRequest);

  // Extract path.
  int pathStart = request.find("/");
  int pathEnd = request.find(" ", pathStart);
  path_ = request.substr(pathStart, pathEnd - pathStart );
  if (path_ == "/")
    path_ = "/index.html";

  // Parse cookies.
  int cookieStart = request.find("Cookie ") + 7;
  int cookieListEnd = request.find("\r\n", cookieStart);
  while (cookieStart < cookieListEnd) {
    int cookieMiddle = request.find("=", cookieStart);
    int cookieEnd = request.find(";", cookieMiddle);
    cookieJar_.insert({request.substr(cookieStart, cookieMiddle - cookieStart),
      request.substr(cookieMiddle + 1, cookieEnd - cookieMiddle - 1)});

    cookieStart = cookieEnd + 2; // plus the '; ' at the end of a cookie.
  }

  // Extract Body.
  int blankLinePos = request.find("\r\n\r\n");
  if (blankLinePos != std::string::npos) {
    int bodyStart = request.find("\r\n\r\n") + 4;
    body_ = request.substr(bodyStart, request.length() - bodyStart + 1);
  }
}
