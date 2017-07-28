#include "httprequest.h"
#include <string>
#include <iostream>
HttpRequest::HttpRequest(const char* rawRequest) {
  std::string request(rawRequest);

  // Extract path.
  int pathStart = request.find("/");
  int pathEnd = request.find(" ", pathStart);
  path_ = request.substr(pathStart, pathEnd - pathStart );
  if (path_ == "/")
    path_ = "/index.html";

  // Parse cookies.
  int cookieTagStart = request.find("Cookie: ");
  if (cookieTagStart != std::string::npos) {
    int cookieStart = cookieTagStart + 8;
    int cookieListEnd = request.find("\r\n", cookieStart);
    while (cookieStart < cookieListEnd) {
      int cookieMiddle = request.find("=", cookieStart);
      int cookieEnd = request.find(";", cookieMiddle);
      if (cookieEnd == std::string::npos)
        cookieEnd = cookieListEnd;
      cookieJar_.insert({request.substr(cookieStart, cookieMiddle - cookieStart),
        request.substr(cookieMiddle + 1, cookieEnd - cookieMiddle - 1)});
      cookieStart = cookieEnd + 2; // plus the '; ' at the end of a cookie.
    }

  }

  // Extract Body.
  int blankLinePos = request.find("\r\n\r\n");
  if (blankLinePos != std::string::npos) {
    int bodyStart = blankLinePos + 4;
    body_ = request.substr(bodyStart);
  }
}

std::string HttpRequest::cookie(std::string flavor) {
  try{
    return cookieJar_.at(flavor);
  } catch(std::out_of_range& e) {
    return std::string("");
  }
}
