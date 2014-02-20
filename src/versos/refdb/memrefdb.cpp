#include "versos/refdb/memrefdb.h"

#include <boost/lexical_cast.hpp>

namespace versos
{
  MemRefDB::MemRefDB()
  {
  }

  MemRefDB::~MemRefDB()
  {
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
    if (uncommitted.isCommitted())
      return -1;

    if (revisions.erase(uncommitted.getId()) != 1)
      return -2;

    return 0;
  }

  const Version& MemRefDB::checkout(const std::string& id)
  {
    if (revisions.find(id) == revisions.end())
      return Version::NOT_FOUND;

    const Version& v = *(revisions.find(id)->second);

    return v;
  }

  Version& MemRefDB::create(const Version& parent, Coordinator& coordinator)
  {
    std::string childId;
    try
    {
      int parentId = boost::lexical_cast<uint64_t>(parent.getId());
      childId = boost::lexical_cast<std::string>(parentId + 1); // TODO: get SHA1
    }
    catch(boost::bad_lexical_cast&)
    {
      return Version::ERROR;
    }

    boost::shared_ptr<Version> v(new Version(childId, parent, coordinator));

    revisions[v->getId()] = v;

    return *v;
  }

  int MemRefDB::lock(const Version&, int)
  {
    return -1;
  }

  int MemRefDB::commit(const Version& v)
  {
    if (v.getParentId() != getHeadId())
      // HEAD changed since @c v's creation, so committing would break the versioning sequence
      return -1;

    headId = v.getId();

    return 0;
  }
}
