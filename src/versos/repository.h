#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "versos/version.h"

#include <string>
#include <map>
#include <set>

// TODO: ifdef WITH_MPI_COORDINATOR
#include <mpi.h>
// TODO: endif

namespace versos
{
  class Coordinator;
  class RefDB;

  /**
   * Options to control the behavior of a repository (passed to @c Repository constructor)
   */
  struct Options
  {
    /**
     * name of coordinator class. Current alternatives:
     *
     *   - @c single
     *   - @c mpi
     *   - @c backend
     */
    std::string coordinator;

    /**
     * name of metadata database class. Current alternatives:
     *
     *   - @c mem
     *   - @c rados
     *   - @c redis
     */
    std::string metadb;

    /**
     * whether to initialize db if empty. Default: false.
     */
    bool metadb_initialize_if_empty;

    /**
     * what to use to seed the internal hashing algorithm.
     */
    std::string hash_seed;

// TODO: ifdef (ENABLE_MPI_COORDINATOR)
    /** options for mpi coordination. @see MpiCoordinator */
    int mpi_leader_rank;
    MPI_Comm mpi_comm;
// TODO: endif

    Options()
    {
      hash_seed = "default seed";
      metadb_initialize_if_empty = false;

// TODO: ifdef (ENABLE_MPI_COORDINATOR)
      mpi_leader_rank = -1;
      mpi_comm = MPI_COMM_NULL;
// TODO: endif
    }
  };

  /**
   * The main interface for checking-out/creating versions.
   */
  class Repository
  {
  private:
    std::string name;
    Coordinator* coordinator;
    RefDB* refdb;

  public:
    static Repository NONE;

    /**
     * creates a new versioning context for the given repository name.
     */
    Repository(const std::string& name, const Options& o);
    Repository();
    ~Repository();

    /**
     * checks whether the repository is empty
     */
    bool isEmpty() const;

    /**
     * initializes a repository.
     */
    int init();

    /**
     * checks out a given version. Returns @c Version::NOT_FOUND if the version doesn't exist.
     */
    const Version& checkout(const std::string& id) const;

    // TODO: checkout() only allows access to committed versions. We can add a retrieve() call that allows to 
    // get a staged versions

    /**
     * returns the latest committed version.
     */
    const Version& checkoutHEAD() const;

    /**
     * creates a new staged version based on the given parent.
     */
    Version& create(const std::string& parentId);

    /**
     * creates a new staged version based on the given parent.
     */
    Version& create(const Version& parent);

    /**
     * Adds an object to the given version. Fails if version is read only.
     */
    int add(Version& v, VersionedObject& o);

    /**
     * removes an object.
     */
    int remove(Version& v, VersionedObject& o);

    /**
     * removes an object.
     */
    int commit(Version& v);

    /**
     */
    void close();

    /**
     */
    const std::string& getName() const;

    /**
     * TODO: currently, every time a coordinator commits, the associated version becomes the HEAD of the repo. 
     * We would like to be more flexible and support:
     *
     *   - push
     *   - pull
     *   - rebase
     *   - merge
     *
     * i.e. multiple users/instances can collaborate on the same repo independently and defer coordination to 
     * later.
     *
     * TODO: of course, we'd like to have branching too. In order to do so, we need a @c Reference object to 
     * represent tags, branches, etc. This introduces a new element in the metadata hierarchy, so things would 
     * look like:
     *
     *   - repository
     *       - reference
     *           - revisions
     *               - objects
     *
     * TODO: differences with Git (more specifically libgit2, although they are in theory equivalent):
     *
     *   - in git, HASH assignment is done at commit-time, not at index (a.k.a. stage) create-time. Git has
     *     an index, which is a container of diffs to which stages modifications. At commit-time it assigns a 
     *     hash to this index. In versos, we currently identify objects by associating the HASH of the staged 
     *     version to the object, so we need to create the new HASH at "index-creation" time (i.e. when a new 
     *     @c Version is instantiated). In terms of abstractions, we have a single @c Version object for both 
     *     reading committed revisions and modifying staged ones; whereas git has two distinct abstraction to 
     *     do this, namely, commits and index, respectively.
     *
     *   - related to the above, we allow to have multiple staging areas (git's index) concurrently, i.e. we
     *     can have multiple @c Version instances and clients can access them concurrently, whereas git (at 
     *     least is default c implementation) assumes only one index exists at any point in time.
     *
     *   - git doesn't coordinate access to the refdb. Multiple instances may leave the refdb in an
     *     inconsistent state. We provide distinct degrees of consistency by providing different ways of 
     *     combining <Coordinator,RefDB,VersionedObject> triplets.
     *
     *   - git rewrites the entire tree (git's internal object metadata hierarchy) associated to a revision
     *     every time it commits. It should be able to do better by snapshotting, e.g. using ZFS/XFS 
     *     capabilities, but it seems it currently doesn't. It's not clear if this is a design issue (whether 
     *     it's internal interfaces assume a POSIX-like backend) or if it's just an implementation issue. In 
     *     either case, we assume snapshotting as a first-class citizen in the @c VersionedObject interface, 
     *     so we take advantage of backends that can provide efficient ways of doing this.
     *
     *   - git uses an object databse, whereas we use @c VersionedObject's
     *
     * TODO: on a more C++-specific note: throughout the library, we don't take ownership of passed pointers. 
     * Instead, we just make copies of @c Coordinator, @c RefDB and @c VersionedObject instances passed by the 
     * user. In this way, the user doesn't have to worry about memory management. This works because the user 
     * isn't sharing anything to the library (and viceversa) via these classes. He/she interacts with the 
     * library via the @c Repository and @c Version classes; instances of the former are owned by the user; of 
     * latter are owned by the library (actually, the user doesn't need to instantiate @c Version objects; we 
     * could actually hide the constructor). In order to completely solve this, we should have factories so 
     * that the user just passes an versos::Repository::Options object containing what s/he wants and the 
     * repository class should construct the objects accordingly.
     *
     * TODO: we can allow to automatically add objects to a version when they get written to.
     *
     *
     * error codes ranges:
     *    0-9  : repo
     *   10-19 : version
     *   20-29 : singleclient
     *   30-39 : radosobj
     *   40-49 : memobj
     *   50-59 : memrefdb
     *   60-69 : mpicoord
     *   70-79 : versionedobject
     *   80-89 : refdb
     *
     * TODO: four options of operation for client-coordinated optimistic mod based on the following:
     *   - synchronization of object metadata has to be done synchronously/async before the commit is 
     *   registered in the metadb
     *   - sync of object metadata has to be done synchronously/async before the next staged version begins to 
     *   be written to
     *
     */
  };
}
#endif
