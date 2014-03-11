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
    if (syncMode == Options::ClientSync::NONE && parent.size() != 0)
      return Version::ERROR;

    return SingleClientCoordinator::create(parent, RefDB::SHARED_LOCK, hashSeed);
  }

  int BackendCoordinator::makeHEAD(const Version& v)
  {
    if (refdb.getLockCount(v, hashSeed) == 0)
      return refdb.makeHEAD(v);
    else
      return 0;
  }
}
