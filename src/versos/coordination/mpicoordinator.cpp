#include "versos/coordination/mpicoordinator.h"

namespace versos
{
  /**
   * MPI coordination has a leader rank that acts like @c SingleClientCoordinator and broadcasts/gathers the 
   * info to/from all the other ranks
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
