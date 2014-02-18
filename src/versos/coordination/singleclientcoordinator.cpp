#include "versos/coordination/singleclientcoordinator.h"

#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  SingleClientCoordinator::SingleClientCoordinator() : refdb(RefDB::NONE)
  {
  }

  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb) : refdb(refdb)
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
    return refdb.checkout(id);
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
      it->snapshot(parent, v);

    return v;
  }

  int SingleClientCoordinator::add(const Version& v, VersionedObject& o)
  {
    o.snapshot(v, checkout(v.getParentId()));
    return 0;
  }

  int SingleClientCoordinator::remove(const Version& v, VersionedObject& o)
  {
    o.remove(v);
    return 0;
  }

  int SingleClientCoordinator::commit(const Version& v)
  {
    refdb.commit(v);

    boost::ptr_set<VersionedObject>::iterator it;

    return 0;
  }

  Coordinator* SingleClientCoordinator::clone() const
  {
    return new SingleClientCoordinator(refdb);
  }

  int SingleClientCoordinator::init()
  {
    return refdb.init();
  }
}
