
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> stringmap;
class HttpRequest
{
 public:
  HttpRequest(const char* rawRequest);
  ~HttpRequest();

  std::string path() { return path_; }

  // Returns the cookie value. flavor: the cookie's name.
  std::string cookie(std::string flavor) { return cookieJar_[flavor]; }

  std::string body() { return body_; }

 private:
  // The request directory.
  std::string path_;
  // A map storing cookies.
  stringmap cookieJar_;
  // Request body.
  string body_;
};