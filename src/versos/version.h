/**
 * version abstraction
 */

#ifndef VERSION_H
#define VERSION_H

#include <string>
#include <stdint.h>
#include <set>

#include <boost/ptr_container/ptr_set.hpp>
#include <boost/serialization/access.hpp>

namespace versos
{
  class VersionedObject;
  class Coordinator;
  class Repository;

  /**
   * interface to a version.
   */
  class Version
  {
    friend class boost::serialization::access;
    friend class Repository;
    friend class Coordinator;
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
    boost::ptr_set<VersionedObject> objects;

  public:
    Version(const std::string& id, const Version& parent);
    Version(
        const std::string& id, const std::string parentId, const boost::ptr_set<VersionedObject>& objects);
    Version(const Version& copy);
    ~Version();

    Status getStatus() const;
    const std::string& getId() const;

    /**
     * number of objects in this version.
     */
    int size() const;

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

    std::ostream& dump(std::ostream& o) const;

  protected:
    /**
     * Adds an object to this version.
     */
    int add(VersionedObject& o);

    /**
     * removes an object.
     */
    int remove(VersionedObject& o);

    /**
     * commits the staged objects.
     */
    void setStatus(Status status);

    /**
     * objects that are part of this version.
     *
     * **NOTE**: only used by Coordinator
     */
    boost::ptr_set<VersionedObject>& getObjects();

  private:
    Version();
    Version(const std::string& id);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int)
    {
      ar & id;
      ar & parentId;
      ar & status;
      ar & objects;
    }
  };

  std::ostream& operator<<(std::ostream& o, const Version& b);
}
#endif
