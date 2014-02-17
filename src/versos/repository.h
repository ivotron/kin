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

#include <boost/shared_ptr.hpp>
#include <rados/librados.hpp>

/**
 */
namespace versos
{
  /**
   * The main interface for checking-out/creating versions.
   */
  class Repository
  {
  private:
    /** @c checkedOutVersions is the main holder of the memory for @c Version 
     * instances. All references to this type of object should be pointing to 
     * its members. This would have to change if we start to evict versions from 
     * the cache, but that's not happening yet.
     */
    std::map<uint64_t, Version> checkedOutVersions;
    std::string name;
    boost::shared_ptr<Coordinator> coordinator;

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
    Version& checkout(uint64_t id);

    /**
     * alias for @c checkout()
     */
    Version& get(uint64_t id);

    /**
     * returns the latest committed version. Returns @c Version::NOT_FOUND if no 
     * version exists in the repository.
     */
    Version& checkoutHEAD();

    /**
     * creates a new version by cloning the given parent.
     */
    Version& create(uint64_t parentId);

    /**
     */
    void close();

    /**
     */
    const std::string& getName() const;
  };
}
#endif
