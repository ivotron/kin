#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <string>

namespace versos
{
  class Version;
  class VersionedObject;
  /**
   * coordinates repository-level operations executed by one or more clients.
   *
   * Implementations go in coordination/ folder.
   */
  class Coordinator
  {
  public:
    virtual ~Coordinator() {}

    /**
     * retrieves latest committed version.
     */
    virtual int getHeadId(std::string& id) = 0;

    /**
     * checks out a version.
     */
    virtual const Version& checkout(const std::string& commitId) = 0;

    // TODO: checkout() only allows access to committed versions. We can add a retrieve() call that allows to 
    // get a staged versions

    /**
     * creates a version based on a given one.
     */
    virtual Version& create(const Version& parent) = 0;

    /**
     * adds an object to a version
     */
    virtual int add(const Version& v, VersionedObject& o) = 0;

    /**
     * removes an object from a version
     */
    virtual int remove(const Version& v, VersionedObject& o) = 0;

    /**
     * commits.
     */
    virtual int commit(const Version& v) = 0;

    /**
     * initializes an empty repo. fails if non-empty.
     */
    virtual int initRepository() = 0;

    /**
     * whether repo is empty.
     */
    virtual bool isRepositoryEmpty() = 0;

    /**
     * copies this object.
     */
    virtual Coordinator* clone() const = 0;
  };
}

#endif
