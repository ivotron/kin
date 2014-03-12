/**
 * version abstraction
 */

#ifndef VERSION_H
#define VERSION_H

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
     * ONLY_ONE is used only by the singleton NOT_FOUND and is used to 
     * communicate when a version hasn't been found.
     */
    enum Status { COMMITTED, STAGED, NONE };
    static Version NOT_FOUND;
    static Version ERROR;
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
     * can I add/remove to this version?
     */
    const std::string& getParentId() const;

    /**
     * can I add/remove to this version?
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

    bool operator== (const Version& other) const;
    bool operator!= (const Version& other) const;

    /**
     * returns @c true if the version is not an instance of @c Version::NOT_FOUND or @c Version::ERROR
     */
    bool isOK() const;

    std::ostream& dump(std::ostream& o) const;

  protected:
    /**
     * Adds an object to this version.
     */
    int add(const VersionedObject& o);

    /**
     * Adds a set of objects to this version.
     */
    int add(const boost::ptr_set<VersionedObject>& o);

    /**
     * removes an object.
     */
    int remove(const VersionedObject& o);

    /**
     * Removes a set of objects to this version.
     */
    int remove(const boost::ptr_set<VersionedObject>& o);

    /**
     * commits the staged objects.
     */
    void setStatus(Status status);

  private:
    Version();
    Version(const std::string& id);
  };

  std::ostream& operator<<(std::ostream& o, const Version& b);
}
#endif
