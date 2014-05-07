/**
 * C++ hiredis wrapper
 *
 * based on the one from: http://thisthread.blogspot.com/2013/04/a-unique-redis-connection.html
 *
 * with modifications/extensions
 */
#if defined(ENABLE_REDIS_METADB) || defined(ENABLE_REDIS_OBJECT)

#ifndef REDISWRAPPER_H
#define REDISWRAPPER_H

#include <iostream>
#include <list>

struct redisContext;

namespace redis
{
class DB
{
private:
  redisContext* ctx;
public:
  DB();
  ~DB();

  bool isConnected() const;
  void connect(const std::string& host) throw();
  void connect(const std::string& host, int port) throw();
  void disconnect();
  void set(const std::string& key, const std::string& value) const throw();
  void del(const std::string& key) const throw();
  void flushall() const throw();
  long long incr(const std::string& key) const throw();
  long long decr(const std::string& key) const throw();
  long long sadd(const std::string& key, const std::string& value) const throw();
  long long sadd(const std::string& key, const std::list<std::string>& values) const throw();
  long long srem(const std::string& key, const std::string& value) const throw();
  long long srem(const std::string& key, const std::list<std::string>& values) const throw();
  std::list<std::string> lrange(const std::string&) const throw();
  std::string get(const std::string& key) const throw();
};

// TODO:
//   - sanity checks for passed keys (we call c_str())

} // namespace Redis
#endif // _H
#endif // ENABLE_
