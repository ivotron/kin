#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <rados/librados.hpp>

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

    virtual ~Coordinator() = 0;

    /**
     * retrieves latest committed version.
     */
    virtual int getHeadId(uint64_t& id) = 0;

    /**
     * checks out a version.
     */
    virtual Version& checkout(uint64_t id) = 0;

    /**
     * creates a version based on a given one.
     */
    virtual Version& create(const Version& parent) = 0;

    /**
     * adds an object to a version
     */
    virtual int add(const Version& v, VersionedObject& o) = 0;

    /**
     * removes an object to a version
     */
    virtual int remove(const Version& v, VersionedObject& o) = 0;

    /**
     * commits.
     */
    virtual int commit(const Version& v) = 0;

    /**
     * copies this object.
     */
    virtual Coordinator* clone() const;
  };
}

#endif
