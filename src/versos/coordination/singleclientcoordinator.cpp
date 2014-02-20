#include "versos/coordination/singleclientcoordinator.h"

#include "versos/refdb/refdb.h"

#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  SingleClientCoordinator::SingleClientCoordinator() : refdb(RefDB::NONE)
  {
  }

  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb) : refdb(refdb)
  {
  }

  SingleClientCoordinator::~SingleClientCoordinator()
  {
  }

  int SingleClientCoordinator::getHeadId(std::string& id)
  {
    id = refdb.getHeadId();

    if (id.empty())
      return -1;

    return 0;
  }

  const Version& SingleClientCoordinator::checkout(const std::string& id)
  {
    const Version& v = refdb.checkout(id);

    if (!v.isCommitted())
      return Version::NOT_COMMITTED;

    return v;
  }

  Version& SingleClientCoordinator::create(const Version& parent)
  {
    if (parent == Version::NOT_FOUND || parent == Version::ERROR)
      return Version::ERROR;

    if (!parent.isCommitted())
      return Version::PARENT_NOT_COMMITTED;

    Version& v = refdb.create(parent, *this);

    if (v == Version::ERROR)
      return v;

    boost::ptr_set<VersionedObject>::iterator it;

    for (it = parent.getObjects().begin(); it != parent.getObjects().end(); ++it)
    {
      if (it->create(parent, v))
      {
        refdb.remove(v);
        return Version::ERROR;
      }
    }

    return v;
  }

  int SingleClientCoordinator::add(const Version& v, VersionedObject& o)
  {
    o.create(checkout(v.getParentId()), v);
    return 0;
  }

  int SingleClientCoordinator::remove(const Version& v, VersionedObject& o)
  {
    o.remove(v);
    return 0;
  }

  int SingleClientCoordinator::commit(const Version& v)
  {
    // TODO: iterate over the objects and call fillSnapshotHoles(parent, child)
    return refdb.commit(v);
  }

  Coordinator* SingleClientCoordinator::clone() const
  {
    return new SingleClientCoordinator(refdb);
  }

  int SingleClientCoordinator::initRepository()
  {
    return refdb.init();
  }

  bool SingleClientCoordinator::isRepositoryEmpty()
  {
    return refdb.isEmpty();
  }
}
