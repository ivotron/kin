/**
 * Versos: VERsioned Object Storage.
 *
 * This library does not perform direct I/O operations into/from the objects 
 * it's managing. It's just a thin layer for grouping objects and handling 
 * version collectively.
 *
 * High-level view of library:
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
 * makes use of the @c snapshot()/remove() functionality of the objdb (implemented through the @c 
 * VersionedObject interface) in order to make sure that object snapshots are kept in sync with the metadata.
 *
 * The user primarly interfaces with @c Repository and @c Version classes. The former to @c checkout() 
 * existing revisions and @c create() new ones based on existing ones; the latter to @c add()/remove() objects 
 * to/from versions.
 *
 * In terms of actual I/O, the user operates on objects directly through the @c VersionedObject interface.
 *
 * The main feature of versos is its pluggability. Depending on which implementations (and their 
 * configuration) of the tree main components of the library (@c Coordinator, @c RefDB and @c VersionedObject) 
 * are selected at instantiation-time, the consistency guarantees change.
 *
 * [^consistency_example]: for example, if two users A,B checkout the same HEAD, when A @c commit()'s its 
 * revision, that revision becomes the new HEAD. What happens if user B tries to commit based on the 
 * not-HEAD-anymore revision? The coordinator should ensure that this doesn't happen.
 */

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "versos/version.h"
#include "versos/coordination/coordinator.h"

#include <string>
#include <map>
#include <set>

#include <boost/scoped_ptr.hpp>

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
    bool isEmpty() const;

    /**
     * initializes a repository.
     */
    int init();

    /**
     * checks out a given version. Returns @c Version::NOT_FOUND if the version 
     * doesn't exist.
     */
    const Version& checkout(const std::string& id) const;

    /**
     * returns the latest committed version. Returns @c Version::NOT_FOUND if no 
     * version exists in the repository.
     */
    const Version& checkoutHEAD() const;

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
     * i.e. multiple users/instances can collaborate on the same repo independently and defer coordination to 
     * later.
     *
     * TODO: of course, we'd like to have branching. In order to do so, we need a @c Reference object to 
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
     *   - in versos, HASH assignment is done at commit-time, not at index (a.k.a. stage) create-time. Git has
     *     an index, which is a container of diffs to which adds/removes objects. At commit-time it assigns a 
     *     hash to it. In versos, we currently identify objects by appending the HASH of the commit to the 
     *     name of the object, so we create the new hash at "index-creation" time (i.e. when a new @c Version 
     *     is instantiated). In terms of abstractions though, we have a single @c Version object for both 
     *     reading committed revisions and modifying uncommitted ones; whereas git has two, commits and index, 
     *     respectively.
     *
     *   - related to the above, we allow to have multiple staging areas (git's indexes) concurrently, i.e. we
     *     can have multiple @c Version instances and clients can access them concurrently, whereas git (at 
     *     least is default c implementation) assumes only one index exists at any point in time. This has the 
     *     consequence of risking consistency (eg. what if two independent instances update the index at the 
     *     same time)?
     *
     *   - git doesn't coordinate access to the refdb. Multiple instances may leave the refdb in an
     *     inconsistent state. We provide distinct degrees of consistency by providing different ways of 
     *     combining <Coordinator,RefDB,VersionedObject> triplets.
     *
     *   - git rewrites trees (git's internal object metadata hierarchy) for every commit (assuming a POSIX
     *     bacckend). It should be able to do better by snapshotting, eg. in ZFS/XFS, but it seems it 
     *     currently doesn't. It's not clear if this is a design issue (whether it's internal interfaces 
     *     assume POSIX-like backend) or if it's just an implementation issue. In either case, we assume 
     *     snapshotting as a first-class citizen in the @c VersionedObject interface, so we take advantage of 
     *     backends that can provide efficient ways of doing this.
     */
  };
}
#endif
