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
  class Coordinator;

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

  private:
    const std::string id;
    const std::string parentId;
    Status status;
    boost::ptr_set<VersionedObject> objects;
    Coordinator* coordinator;

  public:
    Version(std::string id, const Version& parent, Coordinator& c);
    Version(std::string id, const std::string parentId, const boost::ptr_set<VersionedObject>& objects, 
        Coordinator& c);
    Version(const Version& copy);
    ~Version();

    Status getStatus() const;
    const std::string& getId() const;

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
     * commits the staged objects.
     */
    const boost::ptr_set<VersionedObject>& getObjects() const;

    /**
     * can I add/remove to this version?
     */
    const std::string& getParentId() const;

    /**
     * can I add/remove to this version?
     */
    bool isCommitted() const;

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

  private:
    Version(const std::string& id);
  };
}
#endif
