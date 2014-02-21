#include "versos/refdb/memrefdb.h"

#include <openssl/sha.h>

namespace versos
{
  MemRefDB::MemRefDB()
  {
  }

  MemRefDB::~MemRefDB()
  {
  }

  RefDB* MemRefDB::clone()
  {
    return new MemRefDB();
  }

  int MemRefDB::init()
  {
    boost::shared_ptr<Version> v(new Version(Version::PARENT_FOR_ROOT));

    revisions[Version::PARENT_FOR_ROOT.getId()] = v;

    headId = Version::PARENT_FOR_ROOT.getId();

    return 0;
  }

  bool MemRefDB::isEmpty() const
  {
    return revisions.empty();
  }

  const std::string& MemRefDB::getHeadId() const
  {
    return headId;
  };

  int MemRefDB::remove(const Version& uncommitted)
  {
    if (revisions.erase(uncommitted.getId()) != 1)
      return -52;

    return 0;
  }

  const Version& MemRefDB::checkout(const std::string& id)
  {
    if (revisions.find(id) == revisions.end())
      return Version::NOT_FOUND;

    const Version& v = *(revisions.find(id)->second);

    return v;
  }

  Version& MemRefDB::create(const Version& parent, Coordinator& coordinator, const std::string& msg)
  {
    unsigned char childSHA1[20];

    std::string d = parent.getId() + msg;

    SHA1((const unsigned char*) d.c_str(), parent.getId().size(), childSHA1);

    std::string childHash(childSHA1, childSHA1 + 20);

    boost::shared_ptr<Version> v(new Version(childHash, parent, coordinator));

    revisions[v->getId()] = v;

    return *v;
  }

  int MemRefDB::lock(const Version&, int)
  {
    return -53;
  }

  int MemRefDB::commit(const Version& v)
  {
    if (v.getParentId() != getHeadId())
      // HEAD changed since @c v's creation, so committing would break the versioning sequence
      return -54;

    headId = v.getId();

    return 0;
  }
}
