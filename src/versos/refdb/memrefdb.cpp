#include "versos/refdb/memrefdb.h"

#include <boost/lexical_cast.hpp>

namespace versos
{
  MemRefDB::MemRefDB()
  {
  }

  int MemRefDB::init()
  {
    headId = "0";

    return 0;
  }

  const std::string& MemRefDB::getHeadId() const
  {
    return headId;
  };

  const Version& MemRefDB::checkout(const std::string& id)
  {
    if (revisions.find(id) == revisions.end())
      return Version::NOT_FOUND;

    const Version& v = *(revisions.find(id)->second);

    if (!v.isCommitted())
      return Version::NOT_COMMITTED;

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

  int MemRefDB::commit(const Version& v)
  {
    headId = v.getId();

    return 0;
  }
}
