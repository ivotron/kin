#ifndef RADOSREFDB_H
#define RADOSREFDB_H

#include "versos/refdb/refdb.h"

namespace versos
{
  class RadosRefDB : public RefDB
  {
  private:

    // TODO: ideally we'd like to have @c revisions be a KV store instead of a map. Then, a lot of the 
    // functionality implemented on this MemRefDB would move to RefDB and only the access to revisions would 
    // be exposed.
  public:
    MemRefDB(const std::string& repoName, bool persist);
    ~MemRefDB();
    int init();
    int shutdown();
    bool isEmpty() const;
    const std::string& getHeadId() const;
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent, const std::string& msg);
    int remove(const Version& uncommitted);
    int lock(const Version&, int);
    int commit(const Version&);
    RefDB* clone();
  };
}
#endif

