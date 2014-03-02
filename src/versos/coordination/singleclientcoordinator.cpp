#include "versos/coordination/singleclientcoordinator.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"
#include "versos/refdb/refdb.h"

#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb, const Options& o) :
    refdb(refdb), hashSeed(o.hash_seed)
  {
  }

  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb, const std::string& hashSeed) :
    refdb(refdb), hashSeed(hashSeed)
  {
  }

  SingleClientCoordinator::SingleClientCoordinator(RefDB& refdb) :
    refdb(refdb), hashSeed("single")
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

    for (it = getObjects(parent).begin(); it != getObjects(parent).end(); ++it)
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
    // we create without checking what v's state is since Repository is doing all the checks
    return o.create(checkout(v.getParentId()), v);
  }

  int SingleClientCoordinator::remove(Version& v, VersionedObject& o)
  {
    // we remove without checking what v's state is since Repository is doing all the checks
    return o.remove(v);
  }

  int SingleClientCoordinator::makeHEAD(const Version& v)
  {
    return refdb.makeHEAD(v);
  }

  int SingleClientCoordinator::commit(const Version& v)
  {
    // we commit without checking what v's state is since Repository is doing all the checks
    boost::ptr_set<VersionedObject>::iterator o;

    for (o = getObjects(v).begin(); o != getObjects(v).end(); ++o)
      if (o->commit(v))
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
