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
    int add(Version& v);

    // inherited
    int open();
    int close();
    bool isEmpty() const;
    int commit(const Version& v);
    int getLockCount(const Version& v, const std::string& lockKey);
    int makeHEAD(const Version& v);
    int add(const Version& v, const boost::ptr_set<VersionedObject>& o);
    int add(const Version& v, const VersionedObject& o);
    int remove(const Version& v, const VersionedObject& o);
    int remove(const Version& v, const boost::ptr_set<VersionedObject>& o);
  protected:
    int insert(Version& v, LockType lock, const std::string& lockKey);
    Version& get(const std::string& id);
  };
}
#endif
