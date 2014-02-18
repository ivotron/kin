#include "versos/coordination/mpicoordinator.h"

namespace versos
{
  /**
   *  - there is an object (per repo, per revision) that serves as a locker
   *  - we make sure that every rank holds the same revision ID
   *  - using the shared revision ID, every rank places a shared lock on the locker
   *  - every rank writes to objects inside the revision in question
   *  - after a rank is finished adding/modifying a revision, it releases the lock
   *  - it's possible to query the locker to get the number of locks
   *  - a server-side daemon periodically checks this and commits when the lock count gets to zero.
   */

  MpiCoordinator::MpiCoordinator() : leaderRank(-1), comm(MPI_COMM_NULL)
  {
  }

  MpiCoordinator::MpiCoordinator(MPI_Comm comm, int leaderRank) : leaderRank(leaderRank), comm(comm)
  {
  }

  int MpiCoordinator::getHeadId(uint64_t& head)
  {
    return 0;
  }

  Version& MpiCoordinator::checkout(uint64_t id)
  {
  }

  Version& MpiCoordinator::create(const Version& parent)
  {
  }

  int MpiCoordinator::add(const Version& v, const VersionedObject& o)
  {
  }

  int MpiCoordinator::remove(const Version& v, const VersionedObject& o)
  {
  }

  int MpiCoordinator::commit(const Version& v)
  {
    // simple:
    //
    // 1. get the objects that each client has added
    // 2. write that to the metadata object
    // 3. loop on each object and create a snapshot
    // 4. return
    //
    // complex:
    //
    // 1. retrieve the diff in terms of object removal/deletion of each client
    // 2. for each new object, add it to the metadata object
  }

  Coordinator* MpiCoordinator::clone() const
  {
    return new MpiCoordinator(comm, leaderRank);
  }
}

