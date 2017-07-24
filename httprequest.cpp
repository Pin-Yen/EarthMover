#include "httprequest.hpp"
#include <string>

HttpRequest::HttpRequest(const char* rawRequest) {
  string request(rawRequest);

  // Extract path.
  int pathStart = request.find("/");
  int pathEnd = request.find(" ", directoryStart);
  path_ = request.substr(directoryStart, directoryEnd - directoryStart );

  // Parse cookies.
  int cookieStart = request.find("Cookie ") + 7;
  int cookieListEnd = request.find(cookieStart, "\r\n");
  while (cookieStart < cookieListEnd) {
    int cookieMiddle = request.find("=", cookieStart);
    int cookieEnd = request.find(";", cookieMiddle);
    cookieJar_.insert({request.substr(cookieStart, cookieMiddle - cookieStart),
      request.substr(cookieMiddle + 1, cookieEnd - cookieMiddle - 1)});

    cookieStart = cookieEnd + 2; // plus the '; ' at the end of a cookie.
  }

  // Extract Body.
  int bodyStart = request.find("\r\n\r\n") + 4;
  body_ = request.substr(bodyStart, request.length() - bodyStart + 1);
}
