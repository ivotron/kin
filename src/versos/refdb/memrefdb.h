#ifndef MEMREFDB_H
#define MEMREFDB_H

#include "versos/refdb/refdb.h"

#include <map>

#include <boost/shared_ptr.hpp>

namespace versos
{
  class MemRefDB : public RefDB
  {
  private:
    std::map<std::string, boost::shared_ptr<Version> > revisions;
    std::map<std::string, int> locks;
  public:
    MemRefDB(const std::string& repoName);
    virtual ~MemRefDB();

    /**
     * adds a version without changing the HEAD (like @c commit() but without the check and HEAD change).
     */
    void add(Version& v) throw (VersosException);

    // inherited
    void open() throw (VersosException);
    void close() throw (VersosException);
    bool isEmpty() const;
    int commit(const Version& v) throw (VersosException);
    int getLockCount(const Version& v, const std::string& lockKey) throw (VersosException);
    void makeHEAD(const Version& v) throw (VersosException);
    void add(const Version& v, const std::set<std::string>& oid) throw (VersosException);
    void add(const Version& v, const std::string& oid) throw (VersosException);
    void remove(const Version& v, const std::string& oid) throw (VersosException);
    void remove(const Version& v, const std::set<std::string>& oid) throw (VersosException);
  protected:
    void insert(Version& v, LockType lock, const std::string& lockKey) throw (VersosException);
    Version& get(const std::string& id) throw (VersosException);
  };
}
#endif
