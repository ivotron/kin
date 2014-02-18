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
  public:
    MemRefDB();
    ~MemRefDB();
    int init();
    const std::string& getHeadId() const;
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent, Coordinator& coordinator);
    int lock(const Version&, int);
    int commit(const Version&);
  };

  // TODO: RedisRefDB:
  //   - used to lock
}
#endif
