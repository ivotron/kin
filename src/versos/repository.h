/**
 * versioned object storage.
 *
 * This library does not perform direct I/O operations into/from the objects 
 * it's managing. It's just a thin layer for grouping objects and handling 
 * version collectively.
 */

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "versos/version.h"
#include "versos/coordination/coordinator.h"

#include <string>
#include <map>
#include <set>

#include <boost/scoped_ptr.hpp>
#include <rados/librados.hpp>

/**
 * high-level view of library:
 *
 *   - repository
 *       - coordinator
 *           - refdb
 *               - revision
 *                   - revision ids
 *                   - contents of each revision
 *                   - object metadata (name, backend, etc.)
 *          - objdb
 *              - snapshot
 *              - remove
 *
 * a repository just calls to the underlying coordinator. The coordinator uses a refdb to store metadata about 
 * revisions and makes sure that the integrity of the refdb is consistent [^consistency_example]. It also 
 * makes use of the @c snapshot()/remove() functionality of the objectdb (implemented through the @c 
 * VersionedObject interface) in order to make sure that object snapshots are kept in sync with the metadata.
 *
 * The user primarly interfaces with @c Repository and @c Version classes. The former to @c checkout() 
 * existing revisions and @c create() new ones based on existing ones; the latter to @c add()/remove() objects 
 * to/from versions. In terms of actual I/O, she/he operates on objects directly through the @c 
 * VersionedObject interface.
 *
 * The main feature of versos is its pluggability. Depending on which implementations (and their 
 * configuration) of the tree main components of the library (@c Coordinator, @c RefDB and @c VersionedObject) 
 * are selected at instantiation-time, the consistency guarantees change.
 *
 * [^consistency_example]: for example, when a user @c commit()'s a revision and that revision becomes the 
 * head, what happens if another user tries to .
 */
namespace versos
{
  /**
   * The main interface for checking-out/creating versions.
   */
  class Repository
  {
  private:
    std::string name;
    boost::scoped_ptr<Coordinator> coordinator;

  public:
    static Repository NONE;

    /**
     * creates a new versioning context for the given repository name.
     */
    Repository(const std::string& name, Coordinator& c);
    Repository();
    ~Repository();

    /**
     * checks whether the repository is empty
     */
    bool isEmpty();

    /**
     * initializes a repository.
     */
    Version& init();

    /**
     * checks out a given version. Returns @c Version::NOT_FOUND if the version 
     * doesn't exist.
     */
    const Version& checkout(const std::string& id);

    /**
     * returns the latest committed version. Returns @c Version::NOT_FOUND if no 
     * version exists in the repository.
     */
    const Version& checkoutHEAD();

    /**
     * creates a new staged version based on the given parent.
     */
    Version& create(const std::string& parentId);

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
     * i.e. multiple users/instances can collaborate on the same repo.
     *
     * TODO: we need a @c Reference object to represent tags, branches, etc. This introduces a new element in 
     * the metadata hierarchy, so things would look like:
     *
     *   - repository
     *       - reference
     *           - revisions
     *               - objects
     *
     * TODO: differences with Git (more specifically libgit2, although it should be in theory equivalent):
     *
     *   - HASH assignment is done at commit-time, not at index (a.k.a. stage) create-time. Git has an index,
     *     which is a container of diffs and it adds to it and then assigns a hash to it later. Currently, we 
     *     identify objects by appending the HASH of the commit to the name of the object, so we create the 
     *     new hash at "index-creation" time (from a git POV). But this doesn't necessarily have to be like 
     *     this, the only thing that matters is that we can track the changes for a set of objects and then 
     *     later we can assign a hash to them. In terms of abstractions though, I think it's easier to 
     *     interface with a single @c Version object for both reading committed revisions and modifying 
     *     uncommitted ones.
     *   - related to the above, we allow to have multiple staging areas (git's indexes) concurrently, whereas
     *     git (at least is default c implementation) assumes only one index exists. This has the consequence 
     *     of risking consistency (eg. what if two independent instances update the index at the same time)?
     */
  };
}
#endif
