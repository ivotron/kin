#ifndef MPI_COORDINATOR_H
#define MPI_COORDINATOR_H

#include "versos/coordination/singleclientcoordinator.h"

#include <mpi.h>

namespace versos
{
  class RefDB;
  /**
   * coordinates clients that operate concurrently on a repository. MPI coordination has a leader rank that 
   * acts like a @c SingleClientCoordinator and broadcasts/gathers the info to/from all the other ranks
   *
   * TODO: add an option to change the behavior of the coordinator, which can be one of the following:
   *
   *   1. Synchronize objects immediately after they're added. This ensures that two or more ranks don't add 
   *   the same object and thus end up writing to the same object.
   *
   *   2. Synchronize objects after every transaction. This maintains the information of which objects 
   *   are/aren't in which versions, but it doesn't guard against the issue described by 1.
   *
   *   3. Don't synchronize. Currently, this is how the mpi coordinator works. It only keeps information about 
   *   which versions have been committed, but assumes that applications have a way of knowing which objects 
   *   correspond to which versions.
   */
  class MpiCoordinator : public SingleClientCoordinator
  {
  private:
    MPI_Comm comm;
    int leaderRank;
    int myRank;
  public:
    MpiCoordinator();
    MpiCoordinator(const MpiCoordinator&);
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
