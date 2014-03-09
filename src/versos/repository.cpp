#include "versos/repository.h"

#include "versos/options.h"

#include "versos/coordination/singleclientcoordinator.h"

#ifdef ENABLE_MPI_COORDINATOR
  #include "versos/coordination/mpicoordinator.h"
#endif

#include "versos/refdb/memrefdb.h"
#include "versos/refdb/redisrefdb.h"

#include <stdexcept>

namespace versos
{
  Repository::Repository(const std::string& name, const Options& o) : name(name)
  {
    if (o.metadb_type == Options::MetaDB::MEM)
      refdb = new MemRefDB(name + "_metadb");
    else if (o.metadb_type == Options::MetaDB::REDIS)
      refdb = new RedisRefDB(name + "_metadb", o);
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
