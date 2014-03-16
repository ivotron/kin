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
    return create(parent, RefDB::EXCLUSIVE_LOCK, hashSeed);
  }

  Version& SingleClientCoordinator::create(const Version& parent, RefDB::LockType lock, const std::string& key)
  {
    if (syncMode == Options::ClientSync::NONE && parent.size() != 0)
      return Version::ERROR;

    Version& v = refdb.create(parent, hashSeed, lock, key);

    if (v == Version::ERROR)
      return v;

    boost::ptr_set<VersionedObject>::iterator o;

    for (o = v.getParents().begin(); o != v.getParents().end(); ++o)
    {
      if (o->create(parent, v))
        return Version::ERROR;
    }

    if (syncMode == Options::ClientSync::AT_CREATE)
      if (refdb.add(v, v.getParents()))
        return Version::ERROR;

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

    int ret = v.add(o);

    if (ret)
      return ret;

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      ret = refdb.add(v, o);

    if (ret)
      return ret;

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

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      ret = refdb.remove(v, o);

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
    {
      int ret = o->commit(v);

      if (ret < 0)
        return -22;
    }

    if (syncMode == Options::ClientSync::AT_EACH_COMMIT)
    {
      int ret = refdb.add(v, v.getObjects());

      if (ret)
        return ret;
    }

    v.setStatus(Version::COMMITTED);

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
