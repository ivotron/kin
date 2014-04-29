/**
 * version abstraction
 */

#ifndef VERSION_H
#define VERSION_H

#include "versos/versosexception.h"

#include <string>
#include <stdint.h>
#include <set>

namespace versos
{
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
    std::set<std::string> parentObjects;
    std::set<std::string> addedObjects;
    std::set<std::string> removedObjects;

  public:
    Version(const Version& copy);
    Version(const std::string& id, const Version& parent);
    Version(
        const std::string& id, const std::string parentId, const std::set<std::string>& parentObjects);
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
    std::set<std::string> getObjects() const;

    /**
     * Objects that came from parent's version.
     */
    const std::set<std::string>& getParents() const;

    /**
     * elements that have been added to this version w.r.t. the parent's objects.
     */
    const std::set<std::string>& getAdded() const;

    /**
     * elements that have been removed to this version w.r.t. the parent's objects.
     */
    const std::set<std::string>& getRemoved() const;

    /**
     * whether the given object is part of this version
     */
    bool contains(const std::string& oid) const;

    /**
     * returns @c true if the version is not an instance of @c Version::NOT_FOUND
     */
    bool isOK() const;

    bool operator== (const Version& other) const;
    bool operator!= (const Version& other) const;

    std::ostream& dump(std::ostream& oid) const;

  protected:
    /**
     * Adds an object to this version.
     */
    void add(const std::string& oid) throw (VersosException);

    /**
     * Adds a set of objects to this version.
     */
    void add(const std::set<std::string>& oid) throw (VersosException);

    /**
     * removes an object.
     */
    void remove(const std::string& oid) throw (VersosException);

    /**
     * Removes a set of objects to this version.
     */
    void remove(const std::set<std::string>& oid) throw (VersosException);

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
