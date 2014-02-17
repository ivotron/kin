/**
 * version abstraction
 */

#ifndef VERSION_H
#define VERSION_H

#include "versos/coordination/coordinator.h"
#include "versos/objectversioning/versionedobject.h"

#include <string>
#include <stdint.h>
#include <set>

#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  /**
   * interface to a version.
   */
  class Version
  {
  public:
    /**
     * ONLY_ONE is used only by the singleton NOT_FOUND and is used to 
     * communicate when a version hasn't been found.
     */
    enum Status { COMMITTED, STAGED, NONE };
    static Version NOT_FOUND;
    static Version ERROR;
    static Version PARENT_FOR_ROOT;
    static Version NO_VERSION;

  private:
    uint64_t id;
    const Version& parent;
    Status status;
    boost::ptr_set<VersionedObject> objects;
    Coordinator* coordinator;

  public:

    Version(uint64_t id, const Version& parent, Coordinator& c);
    Version();
    Version(const Version& copy);
    ~Version();
    uint64_t getStatus() const;
    uint64_t getId() const;

    /**
     * Adds an object to this version. Fails if version is read only.
     */
    int add(VersionedObject& o);

    /**
     * removes an object.
     */
    int remove(VersionedObject& o);

    /**
     * commits the staged objects.
     */
    int commit();

    /**
     * can I add/remove?
     */
    bool isCommitted() const;
    bool contains(const VersionedObject& o) const;

    bool operator== (const Version& other) const;
    bool operator!= (const Version& other) const;
    bool operator< (const Version& other) const;

  private:
    Version(Status status, uint64_t id);
  };
}

#endif
