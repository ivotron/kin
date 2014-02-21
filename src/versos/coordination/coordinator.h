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
     * retrieves the id of the latest committed version.
     */
    virtual int getHeadId(std::string& id) = 0;

    /**
     * checks out a version.
     *
     * We can safely assume that the returned by this will be checked by @c Repository, so we can return 
     * whatever the underlying DB is giving us.
     */
    virtual const Version& checkout(const std::string& commitId) = 0;

    /**
     * creates a version based on a given one.
     *
     * We can safely assume that the version is safe to be operated on (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Repository should have done this check already.
     */
    virtual Version& create(const Version& parent) = 0;

    /**
     * adds an object to a version
     *
     * We can safely assume that the version is safe to be operated on (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Version should have done this check already.
     */
    virtual int add(const Version& v, VersionedObject& o) = 0;

    /**
     * removes an object from a version
     *
     * We can safely assume that the version is safe to be operated on (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Version should have done this check already.
     */
    virtual int remove(const Version& v, VersionedObject& o) = 0;

    /**
     * commits.
     *
     * We can safely assume that the version is safe to be operated on (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Version should have done this check already.
     */
    virtual int commit(const Version& v) = 0;

    /**
     * initializes an empty repo. fails if non-empty.
     *
     * We can safely assume that the repo is empty since @c Repository should have done this check.
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
