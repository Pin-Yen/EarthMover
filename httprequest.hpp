#ifndef HTTP_REQUEST_
#define HTTP_REQUEST_

#include <fstream>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> stringmap;
class HttpRequest
{
 public:
  HttpRequest(const char* rawRequest);
  ~HttpRequest(){}

  std::string path() { return path_; }

  // Returns the cookie value. flavor: the cookie's name.
  // Returns empty string if not exists.
  std::string cookie(std::string flavor) { return cookieJar_[flavor]; }

  std::string body() { return body_; }

 private:
  // The request directory.
  std::string path_;
  // A map storing cookies.
  stringmap cookieJar_;
  // Request body.
  std::string body_;
};

#endif