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
    std::string headId;
    std::map<std::string, boost::shared_ptr<Version> > revisions;
    // TODO: ideally we'd like to have @c revisions be a KV store instead of a map. Then, a lot of the 
    // functionality implemented on this MemRefDB would move to RefDB and only the access to revisions would 
    // be exposed.
  public:
    MemRefDB();
    ~MemRefDB();
    int init();
    bool isEmpty() const;
    const std::string& getHeadId() const;
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent, Coordinator& coordinator, const std::string& msg);
    int remove(const Version& uncommitted);
    int lock(const Version&, int);
    int commit(const Version&);
  };

}
#endif
