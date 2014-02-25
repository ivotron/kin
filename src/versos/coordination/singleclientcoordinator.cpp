#include "versos/coordination/singleclientcoordinator.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"
#include "versos/refdb/refdb.h"

#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb, const std::string& msg) :
    refdb(refdb), msg(msg)
  {
  }

  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb) :
    refdb(refdb), msg("single")
  {
  }

  SingleClientCoordinator::SingleClientCoordinator(const SingleClientCoordinator& copy) :
    refdb(copy.refdb), msg(copy.msg)
  {
  }

  SingleClientCoordinator::~SingleClientCoordinator()
  {
  }

  int SingleClientCoordinator::getHeadId(std::string& id)
  {
    id = refdb.getHeadId();

    return 0;
  }

  const Version& SingleClientCoordinator::checkout(const std::string& id)
  {
    return refdb.checkout(id);
  }

  Version& SingleClientCoordinator::create(const Version& parent)
  {
    Version& v = refdb.create(parent, msg);

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
    // we add without checking what v's state is since Repository is doing all the checks
    return o.create(checkout(v.getParentId()), v);
  }

  int SingleClientCoordinator::remove(const Version& v, VersionedObject& o)
  {
    // we remove without checking what v's state is since Repository is doing all the checks
    return o.remove(v);
  }

  int SingleClientCoordinator::commit(const Version& v)
  {
    // we commit without checking what v's state is since Repository is doing all the checks
    boost::ptr_set<VersionedObject>::iterator it;

    for (it = v.getObjects().begin(); it != v.getObjects().end(); ++it)
      if (it->commit(v))
        return -22;

    return refdb.commit(v);
  }

  int SingleClientCoordinator::initRepository()
  {
    return refdb.init();
  }

  bool SingleClientCoordinator::isRepositoryEmpty()
  {
    return refdb.isEmpty();
  }

  int SingleClientCoordinator::shutdown()
  {
    return refdb.close();
  }
}
