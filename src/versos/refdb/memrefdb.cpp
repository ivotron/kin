#include "versos/refdb/memrefdb.h"

#include "versos/version.h"

namespace versos
{
  // most of the functions are empty since we're backend-less

  MemRefDB::MemRefDB(const std::string& repoName) : RefDB(repoName)
  {
  }

  MemRefDB::~MemRefDB()
  {
  }

  void MemRefDB::open() throw (VersosException)
  {
  }

  void MemRefDB::close() throw (VersosException)
  {
  }

  bool MemRefDB::isEmpty() const
  {
    return revisions.empty();
  }

  void MemRefDB::makeHEAD(const Version& v) throw (VersosException)
  {
    if (v.getParentId() != headId)
      throw VersosException("current HEAD is not same as version's parent");

    headId = v.getId();
  }

  int MemRefDB::commit(const Version& v) throw (VersosException)
  {
    if (locks.find(v.getId()) == locks.end())
      throw VersosException("Version not found");

    locks[v.getId()] = locks[v.getId()] - 1;

    return locks[v.getId()];
  }

  int MemRefDB::getLockCount(const Version& v, const std::string&) throw (VersosException)
  {
    return locks[v.getId()];
  }

  Version& MemRefDB::get(const std::string& id) throw (VersosException)
  {
    std::map<std::string, boost::shared_ptr<Version> >::iterator found;

    found = revisions.find(id);

    if (found == revisions.end())
      return Version::NOT_FOUND;

    return *(found->second);
  }

  void MemRefDB::add(const Version&, const std::set<std::string>&) throw (VersosException)
  {
  }
  void MemRefDB::remove(const Version&, const std::set<std::string>&) throw (VersosException)
  {
  }
  void MemRefDB::add(const Version&, const std::string&) throw (VersosException)
  {
  }

  void MemRefDB::remove(const Version&, const std::string&) throw (VersosException)
  {
  }

  void MemRefDB::add(Version& v) throw (VersosException)
  {
    RefDB::insert(v);
  }

  void MemRefDB::insert(Version& v, LockType lock, const std::string&) throw (VersosException)
  {
    if (lock == EXCLUSIVE_LOCK)
    {
      if (revisions.find(v.getId()) != revisions.end())
        throw VersosException("version " + v.getId() + " already in metadb");

      if (locks[v.getId()] != 0)
        throw VersosException("version already locked");
    }

    if (locks[v.getId()] > 0 && revisions.find(v.getId()) == revisions.end())
      throw VersosException("version not found");

    if (locks[v.getId()] == 0)
      // add it if it's the first time we see it
      revisions[v.getId()] = boost::shared_ptr<Version>(new Version(v));

    locks[v.getId()] = locks[v.getId()] + 1;
  }
}
