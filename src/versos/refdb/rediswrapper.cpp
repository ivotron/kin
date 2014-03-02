#include "rediswrapper.h"

#include "hiredis.h"
#include "boost/lexical_cast.hpp"

#include <stdexcept>

namespace redis
{

DB::DB()
{
}

DB::~DB()
{
  disconnect();
}

bool DB::isConnected() const
{
  return ctx == NULL ? true : false;
}

void DB::connect(const std::string& host) throw ()
{
  connect(host, 6379);
}

void DB::connect(const std::string& host, int port) throw()
{
  redisContext* context = redisConnect(host.c_str(), port);

  if(!context)
    throw std::runtime_error("No memory for redis");

  if(context->err)
    throw std::runtime_error("Can't connect to Redis");

  ctx.reset(context);
}

void DB::disconnect()
{
  if(!isConnected())
    return;

  redisFree(ctx.get());

  ctx.reset(NULL);
}

void DB::set(const std::string& key, const std::string& value) const throw()
{
  if(isConnected())
    throw std::runtime_error("Not connected");

  void* reply =
    redisCommand(ctx.get(), "SET %b %b", key.c_str(), key.length(), value.c_str(), value.length());

  if(reply)
  {
    freeReplyObject(reply);
    return;
  }

  throw std::runtime_error("No reply from Redis");
}

std::string DB::get(const std::string& key) const throw()
{
  if(isConnected())
    throw std::runtime_error("Not connected");

  redisReply* reply = static_cast<redisReply*>(redisCommand(ctx.get(), "GET %b", key.c_str(), key.length()));

  if(!reply)
    return "";

  std::string result = reply->str ? reply->str : "";
  freeReplyObject(reply);

  return result;
}

} // namespace Redis
