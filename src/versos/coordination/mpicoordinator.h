#ifndef MPI_COORDINATOR_H
#define MPI_COORDINATOR_H

#include "versos/coordination/singleclientcoordinator.h"

#include "mpi.h"

namespace versos
{
  class RefDB;
  /**
   * coordinates clients that operate concurrently on a repository. MPI coordination has a leader rank that 
   * acts like a @c SingleClientCoordinator and broadcasts/gathers the info to/from all the other ranks
   */
  class MpiCoordinator : public SingleClientCoordinator
  {
  private:
    MPI_Comm comm;
    int leaderRank;
    int myRank;
  public:
    MpiCoordinator();
    MpiCoordinator(MPI_Comm comm, int leaderRank, RefDB& refdb, const std::string& msg);
    ~MpiCoordinator();

    // inherited
    int getHeadId(std::string& id);
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent);
    int add(const Version& v, VersionedObject& o);
    int remove(const Version& v, VersionedObject& o);
    int commit(const Version& v);
    int initRepository();
    bool isRepositoryEmpty();
    Coordinator* clone() const;
  };
}
#endif
