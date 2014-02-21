#include "versos/coordination/mpicoordinator.h"

#include "versos/refdb/refdb.h"

#include <boost/ptr_container/ptr_set.hpp>

#include <vector>

namespace versos
{
  MpiCoordinator::MpiCoordinator() :
    SingleClientCoordinator(RefDB::NONE), comm(MPI_COMM_NULL), leaderRank(-1)
  {
  }

  MpiCoordinator::MpiCoordinator(MPI_Comm comm, int leaderRank, RefDB& refdb, const std::string& msg) :
    SingleClientCoordinator(refdb, msg), comm(comm), leaderRank(leaderRank)
  {
    // TODO:
    //
    // if (myRank == leaderRank)
    //   redisdb.init(v)
    //
    // // everybody has a memdb as its refdb, only leader has a valid reference to user-provided redisdb
    // memdb = MemRefDB();

    if(MPI_Comm_rank(comm, &myRank))
      throw "Error getting MPI rank";
  }

  MpiCoordinator::MpiCoordinator(const MpiCoordinator& copy) :
    SingleClientCoordinator(copy.refdb, copy.msg), comm(copy.comm), leaderRank(copy.leaderRank)
  {
  }

  MpiCoordinator::~MpiCoordinator()
  {
  }

  int MpiCoordinator::getHeadId(std::string& id)
  {
    if (myRank == leaderRank && SingleClientCoordinator::getHeadId(id))
        MPI_Abort(comm, -1);

    std::vector<char> writable(id.size() + 1);
    std::copy(id.begin(), id.end(), writable.begin());

    if (MPI_Bcast((void *)&writable[0], 20, MPI_CHAR, leaderRank, comm))
      MPI_Abort(comm, -1);

    id = &writable[0];

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

  Coordinator* MpiCoordinator::clone() const
  {
    return new MpiCoordinator(comm, leaderRank, refdb, msg);
  }
}
