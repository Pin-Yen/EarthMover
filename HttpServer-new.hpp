
class HttpServer
{
 public:
  HttpServer();

  ~HttpServer();

  // starts the HttpServer. (Listens and accepts connections.)
  void run();

 protected:
  // Read request from file discriptor `client` and returns a parsed HttpRequest object.
  HttpRequest readRequest(const int client);

  // Dispatch the HttpRequest to handlers.
  void dispatch(HttpRequest* request);

  // Sends an HTTP response back to the client.
  void sendResponse(const int client, HttpResponse *response);

 private:
  // The limit of pending connections in the queue.
  static const int MAX_CONNECTION_QUEUE_ = 50;
  // The limit of EM instances.
  static const int MAX_EM_INSTANCE_ = 3;
  // Specifies the port to listen.
  static const int PORT_ = 1202;
  // Max characters to read in a single request
  static const int MAX_REQUEST_LENGTH_ = 1500;

  // session to id (session is the string of the http cookie, id is for indexing purposes)
  unordered_map<std::string, int> session2id_;

  // A list storing EarthMover instances.
  AI emList_[MAX_EM_INSTANCE_];

  // Controllers EMs' threads. `False` will stop the thread.
  bool emThreadController_[MAX_EM_INSTANCE_];

};