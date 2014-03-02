#ifndef MEMREFDB_H
#define MEMREFDB_H

#include "versos/refdb/refdb.h"

#include <map>

namespace versos
{
  class MemRefDB : public RefDB
  {
  private:
    std::map<std::string, boost::shared_ptr<Version> > revisions;
  public:
    MemRefDB(const std::string& repoName);
    virtual ~MemRefDB();

    /**
     * adds a version without changing the HEAD (like @c commit() but without the check and HEAD change).
     */
    int add(boost::shared_ptr<Version> v);

    // inherited
    int open();
    int close();
    bool isEmpty() const;
    int commit(const Version& v);
    int makeHEAD(const Version& v);
    const Version& checkout(const std::string& id);
    int remove(const Version& v);
    int addAll(const Version& v);
    int add(const Version& v, const VersionedObject& o);
    int remove(const Version& v, const VersionedObject& o);
  protected:
    int own(boost::shared_ptr<Version> v, LockType lock, const std::string& lockKey);
  };
}
#endif
