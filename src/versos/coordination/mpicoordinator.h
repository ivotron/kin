/**
 * coordinates clients that operate concurrently on a repository.
 */

#ifndef MPI_COORDINATOR_H
#define MPI_COORDINATOR_H

#include "versos/coordination/coordinator.h"

#include "mpi.h"

namespace versos
{
  class MpiCoordinator : public Coordinator
  {
  private:
    int leaderRank;
    MPI_Comm comm;
  public:
    MpiCoordinator();
    MpiCoordinator(MPI_Comm comm, int leaderRank);
    ~MpiCoordinator();

    // inherited
    int getHeadId(uint64_t& id);
    Version& checkout(uint64_t id);
    Version& create(const Version& parent);
    int add(const Version& v, VersionedObject& o);
    int remove(const Version& v, VersionedObject& o);
    int commit(const Version& v);
    Coordinator* clone() const;
  };
}
#endif
