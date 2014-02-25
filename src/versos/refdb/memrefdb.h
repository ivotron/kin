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
    // TODO: ideally we'd like to have @c revisions be a KV store instead of a map. Then, a lot of the 
    // functionality implemented on this MemRefDB would move to RefDB and only the access to revisions would 
    // be exposed.
  public:
    MemRefDB(const std::string& repoName);
    ~MemRefDB();

    // inherited
    int open();
    int close();
    bool isEmpty() const;
    int commit(const Version& v);
    const Version& checkout(const std::string& id);
    int remove(const Version& v);
    int lock(const Version& v, int mode);
  protected:
    int own(boost::shared_ptr<Version> v);
  };
}
#endif
