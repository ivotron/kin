#include "versos/coordination/singleclientcoordinator.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"
#include "versos/refdb/refdb.h"

#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb, const Options& o) :
    refdb(refdb), hashSeed(o.hash_seed), syncMode(o.sync_mode)
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
    Version& v = refdb.create(parent, hashSeed);

    if (v == Version::ERROR)
      return v;

    boost::ptr_set<VersionedObject>::iterator it;

    for (it = v.getParents().begin(); it != v.getParents().end(); ++it)
    {
      if (it->create(parent, v))
      {
        refdb.remove(v);
        return Version::ERROR;
      }
    }

    return v;
  }

  int SingleClientCoordinator::add(Version& v, VersionedObject& o)
  {
    if (syncMode == Options::ClientSync::NONE)
      return -1;

    if (!v.isOK())
      return -1;

    if (v.isCommitted())
      return -2;

    v.add(o);

    return o.create(checkout(v.getParentId()), v);
  }

  int SingleClientCoordinator::remove(Version& v, VersionedObject& o)
  {
    if (syncMode == Options::ClientSync::NONE)
      return -1;

    if (!v.isOK())
      return -1;

    if (v.isCommitted())
      return -2;

    int ret = o.remove(v);

    if (ret)
      return ret;

    return v.remove(o);
  }

  int SingleClientCoordinator::makeHEAD(const Version& v)
  {
    return refdb.makeHEAD(v);
  }

  int SingleClientCoordinator::commit(Version& v)
  {
    if (!v.isOK())
      return -4;

    if (v.isCommitted())
      return -5;

    boost::ptr_set<VersionedObject> objects = v.getObjects();
    boost::ptr_set<VersionedObject>::iterator o;

    for (o = objects.begin(); o != objects.end(); ++o)
      if (o->commit(v))
        return -22;

    v.setStatus(Version::COMMITTED);

    int ret = refdb.commit(v);
    if (ret)
      return ret;

    return 0;
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
