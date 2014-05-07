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
    struct Backend {
      enum Type {
        MEM,
        RADOS,
        REDIS
      };
    };

    /**
     * type of object database to use. Default: metadb::MEM
     */
    Backend::Type objdb_type;
    /**
     * type of metadata database to use. Default: metadb::MEM
     */
    Backend::Type metadb_type;

    /**
     * comma-separated list of URIs. @see http://cpp-netlib.org/0.10.1/in_depth/uri.html
     */
    typedef std::string uri;

    /**
     * address of the metadb server, for remote-based implementations. Default: "".
     */
    uri metadb_server_address;

    /**
     * address of the objdb server, for remote-based implementations. Default: "".
     */
    uri objdb_server_address;

    /**
     * whether to initialize the metadb if empty. Default: false.
     */
    bool metadb_initialize_if_empty;

    /**
     * what to use to seed the internal hashing algorithm. Default: "".
     */
    std::string hash_seed;

    /**
     * when more than one client is modifying the content of a version, this option specifies the level of 
     * synchronization that the coordinator is providing with respect to having each client know about which 
     * objects are being handled by whom (i.e. at what point in time do clients exchange metadata about the 
     * objects being modified by each).
     *
     * @c NONE doesn't share any information. This is useful for applications that "know what they're doing", 
     * which implies that user is responsible for managing object-in-version containment metadata. In other 
     * words, the refdb only stores the parent-child relationship of versions but not of what each version 
     * contains.
     *
     * @c AT_CREATE synchronizes information right after a version has been created, that is, after a 
     * ::Repository::create
     *
     * @c AT_COMMIT synchronizes information right after a version has been committed, that is, after the user 
     * executes ::Repository::commit.
     *
     * @c AT_ADD_OR_REMOVE synchronizes every time an application adds or removes an object from a revision, 
     * i.e. after every @c Repository::add() or @c Repository::remove().
     *
     * Default: @c ClienSync::AT_EACH_COMMIT
     */
    struct ClientSync {
      enum Mode {
        NONE,
        AT_CREATE,
        AT_EACH_COMMIT,
        AT_EACH_ADD_OR_REMOVE
      };
    };

    ClientSync::Mode sync_mode;

    /** For @c MPI coordinator class, this denotes the leader rank options for mpi coordination. Default: 0 */
    int mpi_leader_rank;

    /**
     * MPI communicator used by the @c MPI coordinator class. Assumes that the underlying type representing 
     * communicators is an int. Default: NULL.
     */
    void* mpi_comm;

    /**
     * Rados pool name for the object db. Default: "data".
     */
    std::string rados_objdb_pool_name;

    /**
     * Rados pool name for the metadata db. Default: "data".
     */
    std::string rados_metadb_pool_name;

    Options()
    {
      coordinator_type = Coordinator::SINGLE_CLIENT;
      sync_mode = ClientSync::AT_EACH_COMMIT;

      metadb_type = Backend::MEM;
      metadb_server_address = "";
      metadb_initialize_if_empty = false;

      objdb_type = Backend::MEM;
      objdb_server_address = "";

      hash_seed = "";

      mpi_leader_rank = 0;
      mpi_comm = NULL;

      rados_objdb_pool_name = "data";
      rados_metadb_pool_name = "data";
    }
  };
}
#endif
