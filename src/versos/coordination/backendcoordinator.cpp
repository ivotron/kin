#include "versos/coordination/backendcoordinator.h"

#include "versos/version.h"
#include "versos/objdb/objdb.h"
#include "versos/refdb/refdb.h"

namespace versos
{
  BackendCoordinator::BackendCoordinator(RefDB& refdb, ObjDB& objdb, const Options& o) :
    SingleClientCoordinator(refdb, objdb, o)
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
    std::set<std::string> objects = v.getObjects();
    std::set<std::string>::iterator o;

    for (o = objects.begin(); o != objects.end(); ++o)
      objdb.commit(v, *o);

    if (syncMode == Options::ClientSync::AT_EACH_COMMIT)
      refdb.add(v, v.getObjects());

    v.setStatus(Version::COMMITTED);

    return refdb.commit(v);
  }
}
