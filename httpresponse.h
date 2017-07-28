#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

#include <fstream>
#include <string>

class HttpResponse {
 public:

  // Creates an empty HTTP response. Can optionally set the status code.
  HttpResponse(int httpResponseCode = -1);

  ~HttpResponse();

  // Sets the HTTP response status code.
  HttpResponse& setResponse(int statusCode) {statusCode_ = statusCode; }

  // Explicitly sets the content type.
  HttpResponse& setContentType(const char* type);

  // Sets content-type header according to fileExtension.
  HttpResponse& setContentTypeByFileExt(std::string fileExtension);

  // Populates the response body from the given file.
  HttpResponse& setBody(std::ifstream *file);

  // Popuates the response body from the given string.
  HttpResponse& setBody(std::string body);

  // Appends a cookie to the header.
  HttpResponse& addCookie(const char* name, const char* value);

  // Generates the header string, calcuates contentLength and headerLength.
  // After calling this, the state becomes STATE_COMPILED, which means this response is
  // ready to being served to the client. Further attemps to alter the response data are
  // invalid and will cause the `HTTPResponseException` be thrown.
  HttpResponse& compile();

  // ** The following methods will throw `HTTPResponseException` if the state is STATE_RAW

  std::string getHeaderString();

  int getHeaderLength();

  const char* getBody();

  int getBodyLength();

  int statusCode() {return statusCode_;}

  class HttpResponseException : std::exception {
   public:
    HttpResponseException(const char* description, int line, const char* file) {
      message_.append(description).append("\n\tAt line ").append(std::to_string(line))
      .append(", in ").append(file);
    }

    virtual const char* what() const noexcept {
      return message_.c_str();
    }

   private:
    std::string message_;
  };

 private:
  static const unsigned char STATE_RAW_ = 0;
  static const unsigned char STATE_COMPILED_ = 1;

  // Given a status code, returns its description.
  static const char* status2String(int statusCode);

  std::string header_;

  int statusCode_;
  std::string contentType_;
  std::string contentLength_;
  std::string cookies_;

  bool isBin_;
  char *binBody_;
  std::string body_;

  int bodyLength_;
  int headerLength_;

  // recording the state of this response, should be STATE_RAW_ or STATE_COMPILED_
  unsigned char state_;
};

#endif
