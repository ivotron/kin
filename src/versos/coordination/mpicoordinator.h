#ifndef MPI_COORDINATOR_H
#define MPI_COORDINATOR_H

#include "versos/coordination/singleclientcoordinator.h"

#include "versos/options.h"
#include "versos/refdb/memrefdb.h"

#include <mpi.h>

namespace versos
{
  /**
   * coordinates clients that operate concurrently on a repository. MPI coordination has a leader rank that 
   * acts like a @c SingleClientCoordinator and broadcasts/gathers the info to/from all the other ranks.
   */
  class MpiCoordinator : public SingleClientCoordinator
  {
  private:
    MPI_Comm comm;
    int leaderRank;
    int myRank;
    Options::ClientSync::Mode syncMode;
    MemRefDB localRefDB;
    int size;
  public:
    MpiCoordinator(RefDB& refdb, const Options& o);

    MpiCoordinator(
        MPI_Comm comm,
        int leaderRank,
        Options::ClientSync::Mode syncMode,
        RefDB& refdb,
        const std::string& hashSeed);

    ~MpiCoordinator();

    // inherited
    int getHeadId(std::string& id);
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent);
    int add(Version& v, VersionedObject& o);
    int remove(Version& v, VersionedObject& o);
    int commit(Version& v);
    bool isRepositoryEmpty();
    int shutdown();

  protected:
    // TODO: create a @c MultiClientCoordinator class with the following methods declared as abstract
    // {
    /**
     */
    void broadcast(int* value) const;

    /**
     */
    void broadcast(std::string& id) const;

    /**
     */
    void broadcast(boost::ptr_set<VersionedObject>& objects) const;

    /**
     */
    void allGather(boost::ptr_set<VersionedObject>& objects) const;

    /**
     */
    bool imLeader() const;

    /**
     */
    void handleError(int code) const;

    /**
     */
    void waitForAll() const;
    // }

  private:
    /**
     * initializes the object (common method that multiple constructors call).
     */
    void init();
  };
}
#endif
