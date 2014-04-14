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
    void open() throw (VersosException);
    void close() throw (VersosException);
    bool isEmpty() const throw (VersosException);
    void makeHEAD(const Version& v) throw (VersosException);
    int commit(const Version& v) throw (VersosException);
    int getLockCount(const Version& v, const std::string& lockKey) throw (VersosException);
    void add(const Version& v, const boost::ptr_set<VersionedObject>& o) throw (VersosException);
    void add(const Version& v, const VersionedObject& o) throw (VersosException);
    void remove(const Version& v, const VersionedObject& o) throw (VersosException);
    void remove(const Version& v, const boost::ptr_set<VersionedObject>& o) throw (VersosException);
  protected:
    void insert(Version& v, LockType lock, const std::string& lockKey) throw (VersosException);
    Version& get(const std::string& id) throw (VersosException);
  private:
    int getLockCount(const std::string& id) throw (VersosException);
  };
}
#endif
#endif
