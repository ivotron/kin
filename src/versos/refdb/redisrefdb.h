#ifdef ENABLE_REDIS_METADB

#ifndef REDISREFDB_H
#define REDISREFDB_H

#include "versos/refdb/memrefdb.h"
#include "versos/refdb/rediswrapper.h"

#include <map>

namespace versos
{
  struct Options;

  class RedisRefDB : public MemRefDB
  {
  private:
    std::string host;
    redis::DB redisdb;

  public:
    RedisRefDB(const std::string& repoName, const Options& o);
    virtual ~RedisRefDB();

    // inherited
    int open();
    int close();
    bool isEmpty() const;
    int makeHEAD(const Version& v);
    int commit(const Version& v);
    int getLockCount(const Version& v, const std::string& lockKey);
    int add(const Version& v, const boost::ptr_set<VersionedObject>& o);
    int add(const Version& v, const VersionedObject& o);
    int remove(const Version& v, const VersionedObject& o);
    int remove(const Version& v, const boost::ptr_set<VersionedObject>& o);
  protected:
    int insert(Version& v, LockType lock, const std::string& lockKey);
    Version& get(const std::string& id);
  private:
    int getLockCount(const std::string& id);
  };
}
#endif
#endif
