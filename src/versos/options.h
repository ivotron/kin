#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

namespace versos
{
  /**
   * Options to control the behavior of a repository (passed to @c Repository constructor)
   */
  struct Options
  {
    typedef int MPI_Comm;

    /**
     * supported types of coordinators.
     */
    struct Coordinator {
      enum Type {
        SINGLE_CLIENT,
        MPI,
        BACKEND
      };
    };
    /**
     * type of coordinator to use. Default: coordinator::SINGLE_CLIENT
     */
    Coordinator::Type coordinator_type;

    /**
     * supported types of metadata databases.
     */
    struct MetaDB {
      enum Type {
        MEM,
        RADOS,
        REDIS
      };
    };
    /**
     * type of metadata database to use. Default: metadb::MEM
     */
    MetaDB::Type metadb_type;

    /**
     * whether to initialize the metadb if empty. Default: false.
     */
    bool metadb_initialize_if_empty;

    /**
     * what to use to seed the internal hashing algorithm. Default: ""
     */
    std::string hash_seed;

    /**
     * when more than one client is modifying the content of a version, this option specifies the level of 
     * synchronization that the coordinator is providing with respect to having each client know about which 
     * objects are being handled by whom (i.e. at what point in time do clients exchange metadata about the 
     * objects being modified by each).
     *
     * @c NONE doesn't share any information. This is useful for applications that "know what they're doing". 
     * @c AT_EACH_COMMIT synchronizes information right after a version has been committed, that is, after the 
     * user executes @c Repository::commit(). @c AT_EACH_ADD_OR_REMOVE synchronizes every time an application 
     * adds or removes an object from a revision, i.e. after every @c Repository::add() or @c 
     * Repository::remove().
     *
     * Default: multiclient_sync::NONE
     */
    struct ClientSync {
      enum Mode {
        NONE,
        AT_EACH_COMMIT,
        AT_EACH_ADD_OR_REMOVE
      };
    };

    ClientSync::Mode sync_mode;

    /** For @c MPI coordinator class, this denotes the leader rank options for mpi coordination. Default: 0 */
    int mpi_leader_rank;

    /** MPI communicator used by the @c MPI coordinator class. Default: -1 */
    MPI_Comm mpi_comm;

    Options()
    {
      coordinator_type = Coordinator::SINGLE_CLIENT;
      metadb_type = MetaDB::MEM;

      hash_seed = "";
      metadb_initialize_if_empty = false;
      sync_mode = ClientSync::NONE;

      mpi_leader_rank = 0;
      mpi_comm = -1;
    }
  };
}
#endif
