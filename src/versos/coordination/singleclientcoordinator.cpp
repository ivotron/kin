#include "versos/coordination/singleclientcoordinator.h"

#include "versos/version.h"
#include "versos/obj/object.h"

namespace versos
{
  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb, ObjDB& objdb, const Options& o) :
    refdb(refdb), objdb(objdb), hashSeed(o.hash_seed), syncMode(o.sync_mode)
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

    std::set<std::string>::iterator o;

    for (o = v.getParents().begin(); o != v.getParents().end(); ++o)
      objdb.create(parent, v, *o);

    if (syncMode == Options::ClientSync::AT_CREATE)
      refdb.add(v, v.getParents());

    return v;
  }

  void SingleClientCoordinator::add(Version& v, Object& o) throw (VersosException)
  {
    add(v, o.getId());
  }

  void SingleClientCoordinator::add(Version& v, const std::string& oid) throw (VersosException)
  {
    if (syncMode == Options::ClientSync::NONE)
      throw VersosException("can't add objects to a version in NONE sync_mode");

    if (v.isCommitted())
      throw VersosException("Can't add objects to version; already committed");

    v.add(oid);

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      refdb.add(v, oid);

    objdb.create(checkout(v.getParentId()), v, oid);
  }

  void SingleClientCoordinator::remove(Version& v, Object& o) throw (VersosException)
  {
    if (syncMode == Options::ClientSync::NONE)
      throw VersosException("can't remove objects from a version in NONE sync_mode");

    if (v.isCommitted())
      throw VersosException("Can't remove objects from version; already committed");

    objdb.remove(v, o.getId());

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      refdb.remove(v, o.getId());

    v.remove(o.getId());
  }

  void SingleClientCoordinator::makeHEAD(const Version& v) throw (VersosException)
  {
    refdb.makeHEAD(v);
  }

  int SingleClientCoordinator::commit(Version& v) throw (VersosException)
  {
    if (v.isCommitted())
      throw VersosException("Already committed version");

    std::set<std::string> objects = v.getObjects();
    std::set<std::string>::iterator o;

    for (o = objects.begin(); o != objects.end(); ++o)
      objdb.commit(v, *o);

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
