#include <stdexcept>
#include <string>

#ifndef VERSOS_EXCEPTION_H
#define VERSOS_EXCEPTION_H

class VersosException : public std::runtime_error
{
public:
  VersosException(const std::string& msg) : std::runtime_error(msg) {}
  VersosException(const char* msg) : std::runtime_error(msg) {}
};

#endif
