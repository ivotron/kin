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

  Version& BackendCoordinator::create(const Version& parent) throw (VersosException)
  {
    return SingleClientCoordinator::create(parent, RefDB::SHARED_LOCK, hashSeed);
  }

  void BackendCoordinator::makeHEAD(const Version& v) throw (VersosException)
  {
    if (refdb.getLockCount(v, hashSeed) == 0)
      refdb.makeHEAD(v);
  }

  int BackendCoordinator::commit(Version& v) throw (VersosException)
  {
    if (!v.isOK())
      return -4;

    boost::ptr_set<VersionedObject> objects = v.getObjects();
    boost::ptr_set<VersionedObject>::iterator o;

    for (o = objects.begin(); o != objects.end(); ++o)
      o->commit(v);

    if (syncMode == Options::ClientSync::AT_EACH_COMMIT)
      refdb.add(v, v.getObjects());

    v.setStatus(Version::COMMITTED);

    return refdb.commit(v);
  }
}
