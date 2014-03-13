#include "versos/coordination/backendcoordinator.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"
#include "versos/refdb/refdb.h"

namespace versos
{
  BackendCoordinator::BackendCoordinator(RefDB& refdb, const Options& o) :
    SingleClientCoordinator(refdb, o)
  {
  }

  BackendCoordinator::~BackendCoordinator()
  {
  }

  Version& BackendCoordinator::create(const Version& parent)
  {
    return SingleClientCoordinator::create(parent, RefDB::SHARED_LOCK, hashSeed);
  }

  int BackendCoordinator::makeHEAD(const Version& v)
  {
    if (refdb.getLockCount(v, hashSeed) == 0)
      return refdb.makeHEAD(v);
    else
      return 0;
  }

  int BackendCoordinator::commit(Version& v)
  {
    if (!v.isOK())
      return -4;

    boost::ptr_set<VersionedObject> objects = v.getObjects();
    boost::ptr_set<VersionedObject>::iterator o;

    for (o = objects.begin(); o != objects.end(); ++o)
    {
      int ret = o->commit(v);

      if (ret)
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
}
