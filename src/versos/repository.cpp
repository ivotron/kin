#include "versos/repository.h"

#include "versos/options.h"
#include "versos/coordination/singleclientcoordinator.h"
#include "versos/coordination/mpicoordinator.h"

#include "versos/refdb/memrefdb.h"

#include <stdexcept>

namespace versos
{
  Repository::Repository(const std::string& name, const Options& o) : name(name)
  {
    if (o.metadb_type == Options::MetaDB::MEM)
      refdb = new MemRefDB(name + "_metadb");
    else
      throw std::runtime_error("unknown metadb class");

    // TODO: move this to the coordinator, since the coordinator is the one that owns the responsibility of 
    // talking to the DB
    // {
    refdb->open();

    if (o.metadb_initialize_if_empty == true && refdb->isEmpty() && refdb->init())
      throw std::runtime_error("refdb.init");
    // }

    if (o.coordinator_type == Options::Coordinator::SINGLE_CLIENT)
      coordinator = new SingleClientCoordinator(*refdb, o);
    else if (o.coordinator_type == Options::Coordinator::MPI)
      coordinator = new MpiCoordinator(*refdb, o);
    else
      throw std::runtime_error("unknown coordinator class");

    if (coordinator == NULL || refdb == NULL)
      throw std::runtime_error("none");
  }

  Repository::~Repository()
  {
    delete coordinator;
    delete refdb;
  }

  const std::string& Repository::getName() const
  {
    return name;
  }

  const Version& Repository::checkout(const std::string& id) const
  {
    const Version& v = coordinator->checkout(id);

    if (!v.isCommitted())
      return Version::ERROR;

    return v;
  }

  const Version& Repository::checkoutHEAD() const
  {
    std::string headId;

    if (coordinator->getHeadId(headId))
      return Version::ERROR;

    // TODO: implement a Coordinator::checkoutHEAD() in order to ensure that checking the HEAD and checking it 
    // out is done atomically

    return checkout(headId);
  }

  int Repository::add(Version& v, VersionedObject& o)
  {
    if (!v.isOK())
      return -0;

    if (v.isCommitted())
      return -1;

    v.add(o);

    int ret = coordinator->add(v, o);

    if(ret)
      return ret;

    return 0;
  }

  int Repository::remove(Version& v, VersionedObject& o)
  {
    if (!v.isOK())
      return -2;

    if (v.isCommitted())
      return -3;

    // TODO: check if user has written to the object, in which case we should fail (or not, based on a knob)

    int ret = coordinator->remove(v, o);

    if (ret)
      return ret;

    v.remove(o);

    return 0;
  }

  int Repository::commit(Version& v)
  {
    if (!v.isOK())
      return -4;

    if (v.isCommitted())
      return -5;

    int ret = coordinator->commit(v);

    v.setStatus(Version::COMMITTED);

    if (ret)
      return ret;

    ret = coordinator->makeHEAD(v);

    if (ret)
      return ret;

    return 0;
  }

  bool Repository::isEmpty() const
  {
    return coordinator->isRepositoryEmpty();
  }

  int Repository::init()
  {
    if (!isEmpty())
      return -7;

    return coordinator->initRepository();
  }

  Version& Repository::create(const Version& parent)
  {
    if (!parent.isOK() || !parent.isCommitted())
      return Version::ERROR;

    return coordinator->create(parent);
  }

  Version& Repository::create(const std::string& parentId)
  {
    return coordinator->create(checkout(parentId));
  }

  void Repository::close()
  {
    coordinator->shutdown();
  }
}
