#ifdef ENABLE_MPI_COORDINATOR

#ifndef MPI_COORDINATOR_H
#define MPI_COORDINATOR_H

#include "versos/coordination/singleclientcoordinator.h"

#include "versos/options.h"
#include "versos/refdb/memrefdb.h"

#include <boost/mpi/communicator.hpp>

namespace versos
{
  /**
   * coordinates clients that operate concurrently on a repository. MPI coordination has a leader rank that 
   * acts like a @c SingleClientCoordinator and broadcasts/gathers the info to/from all the other ranks.
   */
  class MpiCoordinator : public SingleClientCoordinator
  {
  private:
    boost::mpi::communicator comm;
    int leaderRank;
    Options::ClientSync::Mode syncMode;
    MemRefDB localRefDB;
  public:
    MpiCoordinator(RefDB& refdb, ObjDB& objdb, const Options& o);

    ~MpiCoordinator();

    // inherited
    std::string getHeadId() throw (VersosException);
    const Version& checkout(const std::string& id) throw (VersosException);
    Version& create(const Version& parent) throw (VersosException);
    void add(Version& v, Object& o) throw (VersosException);
    void add(Version& v, const std::string& o) throw (VersosException);
    void remove(Version& v, Object& o) throw (VersosException);
    void remove(Version& v, const std::string& oid) throw (VersosException);
    int commit(Version& v) throw (VersosException);
    void makeHEAD(const Version& v) throw (VersosException);
    void initRepository() throw (VersosException);
    void openMetaDB() const throw (VersosException);
    bool isRepositoryEmpty() throw (VersosException);
    void shutdown() throw (VersosException);

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
    void broadcast(std::set<std::string>& objects) const;

    /**
     * Syncs the current given version with other clients.
     */
    void allGather(Version& v) const;

    /**
     */
    bool imLeader() const;

    /**
     */
    void handleError(int code) const;

    /**
     */
    void barrier() const;
    // }
  };
}
#endif
#endif
