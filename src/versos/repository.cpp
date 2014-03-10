#include "versos/repository.h"

#include "versos/options.h"

// coordinators
#include "versos/coordination/singleclientcoordinator.h"
#include "versos/coordination/backendcoordinator.h"

#ifdef ENABLE_MPI_COORDINATOR
  #include "versos/coordination/mpicoordinator.h"
#endif

// metadata backends
#include "versos/refdb/memrefdb.h"

#ifdef ENABLE_REDIS_METADB
  #include "versos/refdb/redisrefdb.h"
#endif

#include <stdexcept>

namespace versos
{
  Repository::Repository(const std::string& name, const Options& o) : name(name)
  {
    if (o.metadb_type == Options::MetaDB::MEM)
      refdb = new MemRefDB(name + "_metadb");
#ifdef ENABLE_REDIS_METADB
    else if (o.metadb_type == Options::MetaDB::REDIS)
      refdb = new RedisRefDB(name + "_metadb", o);
#endif
    else
      throw std::runtime_error("unknown metadb class");

    refdb->open();

    if (o.metadb_initialize_if_empty == true && refdb->isEmpty() && refdb->init())
      throw std::runtime_error("refdb.init");

    if (o.coordinator_type == Options::Coordinator::SINGLE_CLIENT)
      coordinator = new SingleClientCoordinator(*refdb, o);
    else if (o.coordinator_type == Options::Coordinator::BACKEND)
      coordinator = new BackendCoordinator(*refdb, o);
#ifdef ENABLE_MPI_COORDINATOR
    else if (o.coordinator_type == Options::Coordinator::MPI)
      coordinator = new MpiCoordinator(*refdb, o);
#endif
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
    return coordinator->checkout(id);
  }

  const Version& Repository::checkoutHEAD() const
  {
    std::string headId;

    if (coordinator->getHeadId(headId))
      return Version::ERROR;

    return checkout(headId);
  }

  int Repository::add(Version& v, VersionedObject& o)
  {
    int ret = coordinator->add(v, o);

    if(ret)
      return ret;

    return 0;
  }

  int Repository::remove(Version& v, VersionedObject& o)
  {
    int ret = coordinator->remove(v, o);

    if (ret)
      return ret;

    return 0;
  }

  int Repository::commit(Version& v)
  {
    int ret = coordinator->commit(v);

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
