#include "versos/coordination/mpicoordinator.h"

#include "versos/refdb/refdb.h"

#include <boost/ptr_container/ptr_set.hpp>

#include <vector>

#include <stdexcept>

namespace versos
{
  MpiCoordinator::MpiCoordinator(RefDB& refdb, const Options& o) :
    SingleClientCoordinator(refdb, o.hash_seed), comm(o.mpi_comm), leaderRank(o.mpi_leader_rank), 
    syncMode(o.sync_mode)
  {
    init();
  }

  MpiCoordinator::MpiCoordinator(
      MPI_Comm comm,
      int leaderRank,
      Options::ClientSync::Mode syncMode,
      RefDB& refdb,
      const std::string& msg) :
    SingleClientCoordinator(refdb, msg), comm(comm), leaderRank(leaderRank), syncMode(syncMode)
  {
    init();
  }

  void MpiCoordinator::init()
  {
    // TODO:
    //
    // if (myRank == leaderRank)
    //   redisdb.init(v)
    //
    // // everybody has a memdb as its refdb, only leader has a valid reference to user-provided redisdb
    // memdb = MemRefDB();
    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      throw std::runtime_error("Not supported yet");

    if (comm == MPI_COMM_NULL || comm == -1)
      throw std::runtime_error("MPI comm is NULL");

    int size;

    if(MPI_Comm_size(comm, &size))
      throw std::runtime_error("unable to get size of MPI comm");

    if (leaderRank < 0)
      throw std::runtime_error("leader rank should be positive");

    if (leaderRank > (size - 1))
      throw std::runtime_error("leader rank shouldn't be greater than size of MPI communicator");

    if(MPI_Comm_rank(comm, &myRank))
      throw std::runtime_error("unable to get MPI rank");
  }

  MpiCoordinator::~MpiCoordinator()
  {
  }

  int MpiCoordinator::getHeadId(std::string& id)
  {
    std::vector<char> id_vec(40);

    if (myRank == leaderRank)
    {
      if (SingleClientCoordinator::getHeadId(id))
        MPI_Abort(comm, -100);

      id_vec = std::vector<char>(id.begin(), id.end());
    }

    id_vec.push_back('\0');

    if (MPI_Bcast(&id_vec[0], 40, MPI_CHAR, leaderRank, comm))
      MPI_Abort(comm, -1);

    id = &id_vec[0];

    return 0;
  }

  const Version& MpiCoordinator::checkout(const std::string& id)
  {
    // TODO:
    // - if memdb returns not_found, we need to contact the leader and ask him to retrieve it from redis

    return SingleClientCoordinator::checkout(id);
  }

  Version& MpiCoordinator::create(const Version& parent)
  {
    // TODO:
    // - we DON'T need to check anything here, since in order to have a valid parent, the user must have 
    // checked out it previously, thus we handle the issue of not having it at the local memdb in the 
    // checkout() call above

    return SingleClientCoordinator::create(parent);
  }

  int MpiCoordinator::add(const Version& v, VersionedObject& o)
  {
    return SingleClientCoordinator::add(v, o);
  }

  int MpiCoordinator::remove(const Version& v, VersionedObject& o)
  {
    return SingleClientCoordinator::remove(v, o);
  }

  int MpiCoordinator::commit(const Version& v)
  {
    // TODO:
    // - use MPI_Reduceall to synchronizes all ranks' local versions
    // - then, do:
    //    if (myRank == leaderRank)
    //      redisdb.commit(v)
    // - and then:

    int ret = refdb.commit(v);

    if (ret)
      return ret;

    if (MPI_Barrier(comm))
      return -62;

    return 0;
  }

  int MpiCoordinator::initRepository()
  {
    // TODO:
    // if (myRank == leaderRank)
    //   redisdb.init(v)

    return refdb.init();
  }

  bool MpiCoordinator::isRepositoryEmpty()
  {
    int isEmpty;

    if (myRank == leaderRank)
        if(refdb.isEmpty())
          isEmpty = 0;
        else
          isEmpty = 1;

    if (MPI_Bcast(&isEmpty, 1, MPI_INT, leaderRank, comm))
      MPI_Abort(comm, -1);

    return isEmpty == 0;
  }

  int MpiCoordinator::shutdown()
  {
    if (myRank == leaderRank)
      return refdb.close();

    return 0;
  }
}
