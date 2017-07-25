#include "httpresponse.hpp"


HttpResponse::HttpResponse(int httpResponseCode) {
  state_ = STATE_RAW_;
  statusCode_ = httpResponseCode;
  binBody_ = NULL;
  isBin_ = false;
}

HttpResponse::~HttpResponse() {
  if (binBody_ != NULL)
    delete[] binBody_;
}

HttpResponse& HttpResponse::setContentType(const char* type) {
  if (state_ == STATE_COMPILED_)
    throw HttpResponseException("Data of a compiled response cannot be modified !", __LINE__, __FILE__);

  contentType_.assign(type);

  return *this;
}

HttpResponse& HttpResponse::setContentTypeByFileExt(std::string fileExtension) {
  if (state_ == STATE_COMPILED_)
    throw HttpResponseException("Data of a compiled response cannot be modified !", __LINE__, __FILE__);

  if (fileExtension == ".png")
    contentType_ = "image/png";
  else if (fileExtension == ".js")
    contentType_ = "application/javascript";
  else if (fileExtension == ".html")
    contentType_ = "text/html";
  else if(fileExtension == ".css")
    contentType_ = "text/css";
  else
    throw HttpResponseException("Unrecognized file type.", __LINE__, __FILE__);

  return *this;
}

HttpResponse& HttpResponse::setBody(std::ifstream *file) {
  if (state_ == STATE_COMPILED_)
    throw HttpResponseException("Data of a compiled response cannot be modified !", __LINE__, __FILE__);

  isBin_ = true;

  file->seekg(0, std::ios_base::end);
  int fileLength = (int)file->tellg();
  bodyLength_ = fileLength;

  if (binBody_ != NULL) {
    delete binBody_;
  }
  binBody_ = new char[fileLength];

  file->seekg(0, std::ios_base::beg);
  file->read(binBody_, fileLength);

  return *this;
}

HttpResponse& HttpResponse::setBody(std::string body) {
  if (state_ == STATE_COMPILED_)
    throw HttpResponseException("Data of a compiled response cannot be modified !", __LINE__, __FILE__);

  body_ = body;
  bodyLength_ = body_.length();

  return *this;
}

HttpResponse& HttpResponse::addCookie(const char* name, const char* value) {
  if (state_ == STATE_COMPILED_)
    throw HttpResponseException("Data of a compiled response cannot be modified !", __LINE__, __FILE__);

  cookies_ += name;
  cookies_ += '=';
  cookies_ += value;
  cookies_ += ';';

  return *this;
}

HttpResponse& HttpResponse::compile() {
  header_ = "";

  // status line
  header_ += "HTTP/1.1 ";
  header_ += std::to_string(statusCode_);
  header_ += " ";
  header_ += HttpResponse::status2String(statusCode_);
  header_ += "\r\n";

  // content type
  header_ += "Content-Type: ";
  header_ += contentType_;
  header_ += "\r\n";

  // content length
  header_ += "Content-Length: ";
  header_ += std::to_string(bodyLength_);
  header_ += "\r\n";

  // cookies
  if (cookies_ != "") {
    header_ += "Set-Cookie: ";
    header_ += cookies_;
  }

  // Connection Closed
  header_ += "Connection: close\r\n";




  state_ = STATE_COMPILED_;
  return *this;
}

std::string HttpResponse::getHeaderString(){
  if (state_ != STATE_COMPILED_)
    throw HttpResponseException("Response not compiled yet", __LINE__, __FILE__);

  return header_;
}

const char* HttpResponse::getBody() {
  if (state_ != STATE_COMPILED_)
    throw HttpResponseException("Response not compiled yet", __LINE__, __FILE__);

  if (isBin_)
    return binBody_;
  else
    return body_.c_str();
}
int HttpResponse::getHeaderLength() {
  if (state_ != STATE_COMPILED_)
    throw HttpResponseException("Response not compiled yet", __LINE__, __FILE__);

  return headerLength_;
}

int HttpResponse::getBodyLength() {
  if (state_ != STATE_COMPILED_)
    throw HttpResponseException("Response not compiled yet", __LINE__, __FILE__);

  return bodyLength_;
}

const char* HttpResponse::status2String(int statusCode) {
  switch (statusCode) {
    case 200: return "OK";
    case 204: return "No Content";
    case 400: return "Bad Request";
    // case 401: return "Unauthorized";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 500: return "Internal Server Error";
    case 503: return "Service Unavailable";
  }

  if (statusCode == -1)
    throw HttpResponseException("Response status code is not set", __LINE__, __FILE__);

  throw HttpResponseException(("Status " + std::to_string(statusCode) + " is not supported").c_str(), __LINE__\
    , __FILE__);
}

