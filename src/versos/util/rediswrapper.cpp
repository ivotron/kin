#include "rediswrapper.h"

#include "hiredis/hiredis.h"

#include <sstream>
#include <stdexcept>

namespace redis
{

DB::DB() : ctx(NULL)
{
}

DB::~DB()
{
  disconnect();
}

bool DB::isConnected() const
{
  return ctx != NULL ? true : false;
}

void DB::connect(const std::string& host) throw ()
{
  connect(host, 6379);
}

void DB::connect(const std::string& host, int port) throw()
{
  if (isConnected())
    disconnect();

  redisContext* context = redisConnect(host.c_str(), port);

  if(!context)
    throw std::runtime_error("No memory for redis");

  if(context->err)
    throw std::runtime_error("Can't connect to Redis");

  ctx = context;
}

void DB::disconnect()
{
  if (ctx == NULL)
    return;

  redisFree(ctx);

  ctx = NULL;
}

long long DB::incr(const std::string& key) const throw()
{
  if (!isConnected())
    throw std::runtime_error("Not connected");

  redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "INCR %b", key.c_str(), key.length()));

  if(!reply)
    throw std::runtime_error("INCR: No reply from Redis");

  long long result = reply->integer;
  freeReplyObject(reply);

  return result;
}

long long DB::decr(const std::string& key) const throw()
{
  if (!isConnected())
    throw std::runtime_error("Not connected");

  redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "DECR %b", key.c_str(), key.length()));

  if(!reply)
    throw std::runtime_error("DECR: No reply from Redis");

  long long result = reply->integer;
  freeReplyObject(reply);

  return result;
}

long long DB::sadd(const std::string& key, const std::list<std::string>& values) const throw()
{
  if (!isConnected())
    throw std::runtime_error("Not connected");

  std::string command;
  long long result = 0;

  command = "SADD " + key + " ";

  for (std::list<std::string>::const_iterator it = values.begin(); it != values.end(); ++it)
  {
    // TODO: redis 2.4+ supports multiple values in a single call.
    redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, (command + *it).c_str()));

    if(!reply)
      throw std::runtime_error("SADD: No reply from Redis");

    result += reply->integer;
    freeReplyObject(reply);
  }

  return result;
}

long long DB::srem(const std::string& key, const std::list<std::string>& values) const throw()
{
  if (!isConnected())
    throw std::runtime_error("Not connected");

  std::stringstream command;

  command << "SREM " << key;

  for (std::list<std::string>::const_iterator it = values.begin(); it != values.end(); ++it)
    command << *it;

  redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, command.str().c_str()));

  if(!reply)
    throw std::runtime_error("SREM: No reply from Redis");

  long long result = reply->integer;
  freeReplyObject(reply);

  return result;
}

long long DB::srem(const std::string& key, const std::string& value) const throw()
{
  if (!isConnected())
    throw std::runtime_error("Not connected");

  redisReply* reply = static_cast<redisReply*>(
      redisCommand(ctx, "SREM %b %b", key.c_str(), key.length(), value.c_str(), value.length()));

  if(!reply)
    throw std::runtime_error("SREM: No reply from Redis");

  long long result = reply->integer;
  freeReplyObject(reply);

  return result;
}

long long DB::sadd(const std::string& key, const std::string& value) const throw()
{
  if (!isConnected())
    throw std::runtime_error("Not connected");

  redisReply* reply = static_cast<redisReply*>(
      redisCommand(ctx, "SADD %b %b", key.c_str(), key.length(), value.c_str(), value.length()));

  if(!reply)
    throw std::runtime_error("SADD: No reply from Redis");

  long long result = reply->integer;
  freeReplyObject(reply);

  return result;
}

std::list<std::string> DB::lrange(const std::string& key) const throw()
{
  if (!isConnected())
    throw std::runtime_error("Not connected");

  redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "LRANGE %b", key.c_str(), key.length()));

  if(!reply)
    throw std::runtime_error("LRANGE: No reply from Redis");

  std::list<std::string> list;

  if (reply->type == REDIS_REPLY_ARRAY)
    for (unsigned int j = 0; j < reply->elements; j++)
      list.push_back(reply->element[j]->str);

  freeReplyObject(reply);

  return list;
}

void DB::set(const std::string& key, const std::string& value) const throw()
{
  if(!isConnected())
    throw std::runtime_error("Not connected");

  void* reply =
    redisCommand(ctx, "SET %b %b", key.c_str(), key.length(), value.c_str(), value.length());

  if(reply)
  {
    freeReplyObject(reply);
    return;
  }

  throw std::runtime_error("SET: No reply from Redis");
}

void DB::flushall() const throw()
{
  if(!isConnected())
    throw std::runtime_error("Not connected");

  void* reply = redisCommand(ctx, "FLUSHALL");

  if(reply)
  {
    freeReplyObject(reply);
    return;
  }

  throw std::runtime_error("FLUSHALL: No reply from Redis");
}

void DB::del(const std::string& key) const throw()
{
  if(!isConnected())
    throw std::runtime_error("Not connected");

  void* reply = redisCommand(ctx, "DEL %b %b", key.c_str(), key.length());

  if(reply)
  {
    freeReplyObject(reply);
    return;
  }

  throw std::runtime_error("DEL: No reply from Redis");
}

std::string DB::get(const std::string& key) const throw()
{
  if(!isConnected())
    throw std::runtime_error("Not connected");

  redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "GET %b", key.c_str(), key.length()));

  if(!reply)
    throw std::runtime_error("GET: No reply from Redis");

  std::string result = reply->str ? reply->str : "";
  freeReplyObject(reply);

  return result;
}

} // namespace Redis
