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

  std::string SingleClientCoordinator::getHeadId() throw (VersosException)
  {
    return refdb.getHeadId();
  }

  const Version& SingleClientCoordinator::checkout(const std::string& id) throw (VersosException)
  {
    return refdb.checkout(id);
  }

  Version& SingleClientCoordinator::create(const Version& parent) throw (VersosException)
  {
    return create(parent, RefDB::EXCLUSIVE_LOCK, hashSeed);
  }

  Version& SingleClientCoordinator::create(
      const Version& parent, RefDB::LockType lock, const std::string& key) throw (VersosException)
  {
    if (syncMode == Options::ClientSync::NONE && parent.size() != 0)
      throw VersosException("parent should be empty for ClientSync::Mode == NONE");

    Version& v = refdb.create(parent, hashSeed, lock, key);

    boost::ptr_set<VersionedObject>::iterator o;

    for (o = v.getParents().begin(); o != v.getParents().end(); ++o)
      o->create(parent, v);

    if (syncMode == Options::ClientSync::AT_CREATE)
      refdb.add(v, v.getParents());

    return v;
  }

  void SingleClientCoordinator::add(Version& v, VersionedObject& o) throw (VersosException)
  {
    if (syncMode == Options::ClientSync::NONE)
      throw VersosException("can't add objects to a version in NONE sync_mode");

    if (v.isCommitted())
      throw VersosException("Can't add objects to version; already committed");

    v.add(o);

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      refdb.add(v, o);

    o.create(checkout(v.getParentId()), v);
  }

  void SingleClientCoordinator::remove(Version& v, VersionedObject& o) throw (VersosException)
  {
    if (syncMode == Options::ClientSync::NONE)
      throw VersosException("can't remove objects from a version in NONE sync_mode");

    if (v.isCommitted())
      throw VersosException("Can't remove objects from version; already committed");

    o.remove(v);

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      refdb.remove(v, o);

    v.remove(o);
  }

  void SingleClientCoordinator::makeHEAD(const Version& v) throw (VersosException)
  {
    refdb.makeHEAD(v);
  }

  int SingleClientCoordinator::commit(Version& v) throw (VersosException)
  {
    if (v.isCommitted())
      throw VersosException("Already committed version");

    boost::ptr_set<VersionedObject> objects = v.getObjects();
    boost::ptr_set<VersionedObject>::iterator o;

    for (o = objects.begin(); o != objects.end(); ++o)
      o->commit(v);

    if (syncMode == Options::ClientSync::AT_EACH_COMMIT)
      refdb.add(v, v.getObjects());

    v.setStatus(Version::COMMITTED);

    return refdb.commit(v);
  }

  void SingleClientCoordinator::initRepository() throw (VersosException)
  {
    refdb.init();
  }

  bool SingleClientCoordinator::isRepositoryEmpty()
  {
    return refdb.isEmpty();
  }

  void SingleClientCoordinator::shutdown() throw (VersosException)
  {
    refdb.close();
  }
}
