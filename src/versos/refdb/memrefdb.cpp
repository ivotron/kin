#include "versos/refdb/memrefdb.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"

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

  int MemRefDB::commit(const Version& v)
  {
    if (locks.find(v.getId()) == locks.end())
      return -55;

    locks[v.getId()] = locks[v.getId()] - 1;

    return locks[v.getId()];
  }

  int MemRefDB::getLockCount(const Version& v, const std::string&)
  {
    return locks[v.getId()];
  }

  Version& MemRefDB::get(const std::string& id)
  {
    if (revisions.find(id) == revisions.end())
      return Version::NOT_FOUND;

    return *(revisions.find(id)->second);
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

  int MemRefDB::add(Version& v)
  {
    return RefDB::insert(v);
  }

  int MemRefDB::insert(Version& v, LockType lock, const std::string&)
  {
    if (lock == EXCLUSIVE_LOCK)
    {
      if (locks[v.getId()] != 0)
        return -56;

      if (revisions.find(v.getId()) != revisions.end())
        return -57;
    }

    locks[v.getId()] = locks[v.getId()] + 1;

    revisions[v.getId()] = boost::shared_ptr<Version>(new Version(v));

    return 0;
  }
}
