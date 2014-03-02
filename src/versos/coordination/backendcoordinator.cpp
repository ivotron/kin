#include "versos/coordination/backendcoordinator.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"
#include "versos/refdb/refdb.h"

namespace versos
{
  // TODO: currently not fault-tolerant (what if a client dies?)

  BackendCoordinator::BackendCoordinator(RefDB& refdb, const Options& o) :
    SingleClientCoordinator(refdb, o), syncMode(o.sync_mode)
  {
    init();
  }

  BackendCoordinator::BackendCoordinator(
      RefDB& refdb, Options::ClientSync::Mode syncMode, const std::string& hashSeed) : 
    SingleClientCoordinator(refdb, hashSeed), syncMode(syncMode)
  {
    init();
  }

  BackendCoordinator::BackendCoordinator(RefDB& refdb) : SingleClientCoordinator(refdb, "backend")
  {
    init();
  }

  BackendCoordinator::~BackendCoordinator()
  {
  }

  void BackendCoordinator::init()
  {
    // TODO:
    //   - openDB()
    //   - initDB()
  }

  Version& BackendCoordinator::create(const Version& parent)
  {
    Version& newVersion = refdb.create(parent, hashSeed, RefDB::SHARED_LOCK, hashSeed);

    if (syncMode == Options::ClientSync::NONE)
      getObjects(newVersion).clear();

    return newVersion;
  }

  int BackendCoordinator::add(Version& v, VersionedObject& o)
  {
    if (SingleClientCoordinator::add(v, o))
      return -65;

    if (syncMode == Options::ClientSync::NONE)
      getObjects(v).clear();
    else if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      return refdb.add(v, o);

    return 0;
  }

  int BackendCoordinator::remove(Version& v, VersionedObject& o)
  {
    if (SingleClientCoordinator::remove(v, o))
      return -66;

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      return refdb.add(v, o);

    return 0;
  }

  int BackendCoordinator::commit(Version& v)
  {
    int ret;

    if (syncMode == Options::ClientSync::AT_EACH_COMMIT)
    {
      // TODO: get the diff set<Obj> added; set<Obj> removed
      ret = refdb.addAll(v);

      if (ret < 0)
        return -109;
      else if ((unsigned) ret != v.size())
        return -108;
    }

    ret = refdb.commit(v);

    if (ret == 0)
    {
      // 1. Get all objects, instantiate them and then:
      //
      // for (o = getObjects(v).begin(); o != getObjects(v).end(); ++o)
      //   if (o->commit(v))
      //     return -22;

      // "exec SET repoName_versionId_status 'committed'"

      // add to local set<v_id> isCompleted to signal completion
    }

    return 0;
  }

  int BackendCoordinator::makeHEAD(const Version& v)
  {
    // TODO: if (isCompleted(v))
    return refdb.makeHEAD(v);
  }
}
