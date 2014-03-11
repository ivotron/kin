#include "versos/refdb/memrefdb.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"

#include <boost/serialization/shared_ptr.hpp>

namespace versos
{
  // most of the functions are empty since we're backend-less

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

  int MemRefDB::makeHEAD(const Version& v)
  {
    if (v.getParentId() != headId)
      return -54;

    headId = v.getId();

    return 0;
  }

  int MemRefDB::commit(const Version&)
  {
    return 0;
  }

  int MemRefDB::getLockCount(const Version&, const std::string&)
  {
    return -1;
  }

  const Version& MemRefDB::checkout(const std::string& id)
  {
    if (revisions.find(id) == revisions.end())
      return Version::NOT_FOUND;

    const Version& v = *(revisions.find(id)->second);

    if (!v.isCommitted())
      return Version::ERROR;

    return v;
  }

  int MemRefDB::add(const Version&, const boost::ptr_set<VersionedObject>&)
  {
    return 0;
  }
  int MemRefDB::remove(const Version&, const boost::ptr_set<VersionedObject>&)
  {
    return 0;
  }
  int MemRefDB::add(const Version&, const VersionedObject&)
  {
    return 0;
  }

  int MemRefDB::remove(const Version&, const VersionedObject&)
  {
    return 0;
  }

  int MemRefDB::add(boost::shared_ptr<Version> v)
  {
    return RefDB::own(v);
  }

  int MemRefDB::own(boost::shared_ptr<Version> v, LockType lock, const std::string&)
  {
    if (lock == SHARED_LOCK)
      // can't be shared
      return -56;

    if (revisions.find(v->getId()) != revisions.end())
      // re-adding an already-added version; or (very unlikely) there is a hash collision
      return -57;

    revisions[v->getId()] = v;

    return 0;
  }
}
