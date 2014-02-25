#include "versos/refdb/memrefdb.h"

#include "versos/version.h"

#include <boost/serialization/shared_ptr.hpp>

namespace versos
{
  MemRefDB::MemRefDB(const std::string& repoName) : RefDB(repoName)
  {
  }

  MemRefDB::~MemRefDB()
  {
  }

  int MemRefDB::open()
  {
    return 0;
  }

  int MemRefDB::close()
  {
    return 0;
  }

  bool MemRefDB::isEmpty() const
  {
    return revisions.empty();
  }

  int MemRefDB::commit(const Version& v)
  {
    if (v.getParentId() != getHeadId())
      // we only support sequential histories, thus if HEAD changed since the time @c v's was instantiated, 
      // then committing would break the versioning sequence
      return -54;

    headId = v.getId();

    return 0;
  }

  const Version& MemRefDB::checkout(const std::string& id)
  {
    if (revisions.find(id) == revisions.end())
      return Version::NOT_FOUND;

    const Version& v = *(revisions.find(id)->second);

    return v;
  }

  int MemRefDB::remove(const Version& uncommitted)
  {
    if (revisions.erase(uncommitted.getId()) != 1)
      return -52;

    return 0;
  }

  int MemRefDB::lock(const Version&, int)
  {
    return -53;
  }

  int MemRefDB::own(boost::shared_ptr<Version> v)
  {
    if (revisions.find(v->getId()) != revisions.end())
      // re-adding an already-added version; or (very unlikely) there is a SHA1 collision
      return -56;

    revisions[v->getId()] = v;

    return 0;
  }
}
