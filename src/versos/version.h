/**
 * version abstraction
 */

#ifndef VERSION_H
#define VERSION_H

#include "versos/versosexception.h"

#include <string>
#include <stdint.h>
#include <set>

#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  class VersionedObject;
  class SingleClientCoordinator;
  class MpiCoordinator;
  class BackendCoordinator;

  /**
   * interface to a version.
   */
  class Version
  {
    friend class SingleClientCoordinator;
    friend class MpiCoordinator;
    friend class BackendCoordinator;
  public:
    /**
     * NONE is used by the singleton NOT_FOUND; to communicate when a version hasn't been found.
     */
    enum Status { COMMITTED, STAGED, NONE };
    static Version NOT_FOUND;
    static Version PARENT_FOR_ROOT;
  protected:

  private:
    const std::string id;
    const std::string parentId;
    Status status;
    boost::ptr_set<VersionedObject> parentObjects;
    boost::ptr_set<VersionedObject> addedObjects;
    boost::ptr_set<VersionedObject> removedObjects;

  public:
    Version(const Version& copy);
    Version(const std::string& id, const Version& parent);
    Version(
        const std::string& id, const std::string parentId, const boost::ptr_set<VersionedObject>& 
        parentObjects);
    ~Version();

    Status getStatus() const;
    const std::string& getId() const;

    /**
     * number of objects in this version.
     */
    unsigned int size() const;

    /**
     * who's my parent?
     */
    const std::string& getParentId() const;

    /**
     * can I modify to this version?
     */
    bool isCommitted() const;

    /**
     * returns a copy of the set of objects that are part of this version. This is equivalent to doing @code 
     * getParentObjects() - getRemoved() + getAdded() @endcode
     */
    boost::ptr_set<VersionedObject> getObjects() const;

    /**
     * Objects that came from parent's version.
     */
    const boost::ptr_set<VersionedObject>& getParents() const;

    /**
     * elements that have been added to this version w.r.t. the parent's objects.
     */
    const boost::ptr_set<VersionedObject>& getAdded() const;

    /**
     * elements that have been removed to this version w.r.t. the parent's objects.
     */
    const boost::ptr_set<VersionedObject>& getRemoved() const;

    /**
     * whether the given object is part of this version
     */
    bool contains(const VersionedObject& o) const;

    /**
     * returns @c true if the version is not an instance of @c Version::NOT_FOUND
     */
    bool isOK() const;

    bool operator== (const Version& other) const;
    bool operator!= (const Version& other) const;

    std::ostream& dump(std::ostream& o) const;

  protected:
    /**
     * Adds an object to this version.
     */
    void add(const VersionedObject& o) throw (VersosException);

    /**
     * Adds a set of objects to this version.
     */
    void add(const boost::ptr_set<VersionedObject>& o) throw (VersosException);

    /**
     * removes an object.
     */
    void remove(const VersionedObject& o) throw (VersosException);

    /**
     * Removes a set of objects to this version.
     */
    void remove(const boost::ptr_set<VersionedObject>& o) throw (VersosException);

    /**
     * modifies the status of the version.
     */
    void setStatus(Status status);

  private:
    Version();
    Version(const std::string& id);
  };

  std::ostream& operator<<(std::ostream& o, const Version& b);
}
#endif
