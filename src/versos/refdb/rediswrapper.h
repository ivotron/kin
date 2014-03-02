/**
 * C++ hiredis wrapper
 *
 * based on the one from: http://thisthread.blogspot.com/2013/04/a-unique-redis-connection.html
 *
 * with modifications/extensions
 */
#ifndef REDISWRAPPER_H
#define REDISWRAPPER_H

#include <iostream>

#include <boost/scoped_ptr.hpp>

struct redisContext;

namespace redis
{
class DB
{
private:
  boost::scoped_ptr<redisContext> ctx;
public:
  DB();
  ~DB();

  bool isConnected() const;
  void connect(const std::string& host) throw();
  void connect(const std::string& host, int port) throw();
  void disconnect();
  void set(const std::string& key, const std::string& value) const throw();
  std::string get(const std::string& key) const throw();
};
} // namespace Redis
#endif
