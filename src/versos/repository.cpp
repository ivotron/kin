#include "versos/repository.h"

#include "versos/options.h"

// coordinators
#include "versos/coordination/singleclientcoordinator.h"
#include "versos/coordination/backendcoordinator.h"

#ifdef ENABLE_MPI_COORDINATOR
  #include "versos/coordination/mpicoordinator.h"
#endif

// object backends
#include "versos/objdb/memobjdb.h"
#ifdef ENABLE_REDIS_BACKEND
  #include "versos/objdb/redisobjdb.h"
#endif

// metadata backends
#include "versos/refdb/memrefdb.h"
#ifdef ENABLE_REDIS_BACKEND
  #include "versos/refdb/redisrefdb.h"
#endif

namespace versos
{
  Repository::Repository(const std::string& name, const Options& o) : name(name)
  {
    if (o.metadb_type == Options::Backend::MEM)
      refdb = new MemRefDB(name);
#ifdef ENABLE_REDIS_BACKEND
    else if (o.metadb_type == Options::Backend::REDIS)
      refdb = new RedisRefDB(name, o);
#endif
    else
      throw VersosException("unknown metadb class");

    if (o.objdb_type == Options::Backend::MEM)
      objdb = new MemObjDB(name, o);
#ifdef ENABLE_REDIS_BACKEND
    else if (o.metadb_type == Options::Backend::REDIS)
      objdb = new RedisObjDB(name, o);
#endif
    else
      throw VersosException("unknown metadb class");

    if (o.coordinator_type == Options::Coordinator::SINGLE_CLIENT)
      coordinator = new SingleClientCoordinator(*refdb, *objdb, o);
    else if (o.coordinator_type == Options::Coordinator::BACKEND)
      coordinator = new BackendCoordinator(*refdb, *objdb, o);
#ifdef ENABLE_MPI_COORDINATOR
    else if (o.coordinator_type == Options::Coordinator::MPI)
      coordinator = new MpiCoordinator(*refdb, *objdb, o);
#endif
    else
      throw VersosException("unknown coordinator class");

    if (coordinator == NULL || refdb == NULL)
      // sanity check
      throw VersosException("none");

    coordinator->openMetaDB();

    if (o.metadb_initialize_if_empty == true && coordinator->isRepositoryEmpty())
      coordinator->initRepository();
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

  const Version& Repository::checkout(const std::string& id) const throw (VersosException)
  {
    return coordinator->checkout(id);
  }

  const Version& Repository::checkoutHEAD() const throw (VersosException)
  {
    return checkout(coordinator->getHeadId());
  }

  void Repository::add(Version& v, Object& o) throw (VersosException)
  {
    add(v, o.getId());
  }
  void Repository::add(Version& v, const std::string& oid) throw (VersosException)
  {
    coordinator->add(v, oid);
  }

  void Repository::remove(Version& v, Object& o) throw (VersosException)
  {
    remove(v, o.getId());
  }
  void Repository::remove(Version& v, const std::string& oid) throw (VersosException)
  {
    coordinator->remove(v, oid);
  }

  int Repository::commit(Version& v) throw (VersosException)
  {
    int lockCount = coordinator->commit(v);

    if(lockCount == 0)
      coordinator->makeHEAD(v);

    return lockCount;
  }

  bool Repository::isEmpty() const
  {
    return coordinator->isRepositoryEmpty();
  }

  void Repository::init() throw (VersosException)
  {
    coordinator->initRepository();
  }

  Version& Repository::create(const Version& parent) throw (VersosException)
  {
    if (!parent.isCommitted())
      throw VersosException("Parent not committed");

    return coordinator->create(parent);
  }

  Version& Repository::create(const std::string& parentId) throw (VersosException)
  {
    return coordinator->create(checkout(parentId));
  }

  void Repository::close() throw (VersosException)
  {
    coordinator->shutdown();
  }
}
