#include "httprequest.h"
#include <string>
#include <iostream>
HttpRequest::HttpRequest(std::string rawRequest) {

  // Extract path.
  int pathStart = rawRequest.find("/");
  int pathEnd = rawRequest.find(" ", pathStart);
  path_ = rawRequest.substr(pathStart, pathEnd - pathStart);
  if (path_ == "/")
    path_ = "/index.html";

  // Parse cookies.
  int cookieTagStart = rawRequest.find("Cookie: ");
  if (cookieTagStart != std::string::npos) {
    int cookieStart = cookieTagStart + 8;
    int cookieListEnd = rawRequest.find("\r\n", cookieStart);
    while (cookieStart < cookieListEnd) {
      int cookieMiddle = rawRequest.find("=", cookieStart);
      int cookieEnd = rawRequest.find(";", cookieMiddle);
      if (cookieEnd == std::string::npos)
        cookieEnd = cookieListEnd;
      cookieJar_.insert({
          rawRequest.substr(cookieStart, cookieMiddle - cookieStart),
          rawRequest.substr(cookieMiddle + 1, cookieEnd - cookieMiddle - 1)});
      cookieStart = cookieEnd + 2;  // plus the '; ' at the end of a cookie.
    }
  }
  // Parse content-length
  int contentLengthStart = rawRequest.find("Content-Length: ");
  if (contentLengthStart == std::string::npos) {
    contentLength_ = 0;
  } else {
    contentLengthStart += 16;
    int contentLengthEnd = rawRequest.find("\r\n", contentLengthStart);
    contentLength_ = atoi(rawRequest.substr(contentLengthStart, contentLengthEnd - contentLengthStart).c_str());
  }

  // Extract Body if exists.
  int blankLinePos = rawRequest.find("\r\n\r\n");
  if (blankLinePos != std::string::npos) {
    int bodyStart = blankLinePos + 4;
    body_ = rawRequest.substr(bodyStart);
  }
}


std::string HttpRequest::cookie(std::string flavor) {
  try {
    return cookieJar_.at(flavor);
  } catch(std::out_of_range& e) {
    return std::string("");
  }
}
