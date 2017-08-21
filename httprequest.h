#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include <exception>
#include <fstream>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> stringmap;
class HttpRequest {
 public:
  explicit HttpRequest(std::string rawRequest);
  ~HttpRequest() {}

  std::string path() { return path_; }

  // Returns the cookie value. flavor: the cookie's name.
  // Returns empty string if not exists.
  std::string cookie(std::string flavor);

  std::string body() { return body_; }

  int contentLength() { return contentLength_; }
  // Add the body.
  void addBody(std::string body) { body_ = body; }
  class BadRequestException : std::exception {
   public:
    explicit BadRequestException(const char* description) {
      message_.assign(description);
    }

    virtual const char* what() const noexcept {
      return message_.c_str();
    }

   private:
    std::string message_;
  };

 private:
  // The request directory.
  std::string path_;
  // A map storing cookies.
  stringmap cookieJar_;

  int contentLength_;
  // Request body.
  std::string body_;
};

#endif  // HTTPREQUEST_H_
