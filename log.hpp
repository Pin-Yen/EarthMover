#ifndef LOG_H_
#define LOG_H_

#include <fstream>
#include <string>
#include <iostream>
#include <assert.h>

#define FILE_NAME "gamelog.txt"

class Log
{
public:

  static void init(){
    outputFileStream.open(FILE_NAME, std::ofstream::out);
    assert(outputFileStream.is_open());
  }
  static void closeLog(){
    outputFileStream.close();
  }

  static std::ofstream outputFileStream;
};

template <typename T>
Log& operator << (Log& log, const T& data) {
  Log::outputFileStream << data;
  Log::outputFileStream.flush();
  return log;
}

template <typename T>
Log& operator << (Log& log, const T data[]) {
  Log::outputFileStream << data;
  Log::outputFileStream.flush();
  return log;
}

#endif