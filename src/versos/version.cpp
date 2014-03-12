#include "versos/version.h"

#include "versos/coordination/coordinator.h"
#include "versos/objectversioning/versionedobject.h"

namespace versos
{
  Version Version::ERROR           = Version("0000000000000000000000000000000000000001");
  Version Version::NOT_FOUND       = Version("0000000000000000000000000000000000000002");
  Version Version::PARENT_FOR_ROOT = Version("0000000000000000000000000000000000000003");

  Version::Version() : id(""), parentId(""), status(Version::NONE)
  {
  }

  Version::Version(const Version& copy) :
    id(copy.id), parentId(copy.parentId), status(copy.status), parentObjects(copy.parentObjects), 
    addedObjects(copy.addedObjects), removedObjects(copy.removedObjects)
  {
  }

  Version::Version(const std::string& id) : id(id), parentId(""), status(Version::NONE)
  {
    if (id == "0000000000000000000000000000000000000003")
      // special case
      status = COMMITTED;
  }

  Version::Version(const std::string& id, const Version& parent) :
    id(id), parentId(parent.getId()), status(STAGED), parentObjects(parent.getObjects())
  {
  }

  Version::Version(
      const std::string& id, std::string parentId, const boost::ptr_set<VersionedObject>& objects) :
    id(id), parentId(parentId), status(COMMITTED), addedObjects(objects)
  {
  }

  Version::~Version()
  {
  }

  int Version::add(const boost::ptr_set<VersionedObject>& o)
  {
    if (!isOK())
      return -1;

    if (isCommitted())
      return -1;

    boost::ptr_set<VersionedObject>::iterator it;

    for (it = o.begin(); it != o.end(); ++it)
    {
      int ret = add(*it);

      if (ret)
        return ret;
    }

    return 0;
  }

  int Version::remove(const boost::ptr_set<VersionedObject>& o)
  {
    if (!isOK())
      return -1;

    if (isCommitted())
      return -2;

    boost::ptr_set<VersionedObject>::iterator it;

    for (it = o.begin(); it != o.end(); ++it)
    {
      int ret = remove(*it);

      if (ret)
        return ret;
    }

    return 0;
  }

  int Version::add(const VersionedObject& o)
  {
    if (!isOK())
      return -1;

    if (isCommitted())
      return -1;

    if (contains(o))
      return -12;

    addedObjects.insert(o.clone());

    return 0;
  }

  int Version::remove(const VersionedObject& o)
  {
    if (!isOK())
      return -2;

    if (isCommitted())
      return -3;

    // TODO: check if user has written to the object, in which case we should fail (or not, based on a knob)

    if (!contains(o))
      return -15;

    removedObjects.insert(o.clone());

    return 0;
  }

  void Version::setStatus(Status newStatus)
  {
    status = newStatus;
  }

  const std::string& Version::getParentId() const
  {
    return parentId;
  }

  bool Version::isCommitted() const
  {
    return status == COMMITTED;
  }

  bool Version::isOK() const
  {
    return (*this != Version::NOT_FOUND && *this != Version::ERROR);
  }

  bool Version::contains(const VersionedObject& o) const
  {
    if (addedObjects.find(o) != addedObjects.end())
      return true;

    if ((parentObjects.find(o) != parentObjects.end()) && (removedObjects.find(o) == removedObjects.end()) )
      // if it's in parent's and hasn't been removed
      return true;

    return false;
  }

  Version::Status Version::getStatus() const
  {
    return status;
  }

  const std::string& Version::getId() const
  {
    return id;
  }

  unsigned int Version::size() const
  {
    return parentObjects.size() + addedObjects.size() - removedObjects.size();
  }

  const boost::ptr_set<VersionedObject>& Version::getParents() const
  {
    return parentObjects;
  }

  const boost::ptr_set<VersionedObject>& Version::getAdded() const
  {
    return addedObjects;
  }

  const boost::ptr_set<VersionedObject>& Version::getRemoved() const
  {
    return removedObjects;
  }

  /*
   * TODO: the working set is maintained in parent's/added/removed sets of objects. For syncMode == 
   * at-each-add/remove, there's significant overhead if too many objects are added/removed from a transaction 
   * since the same objects are being resent every time. We can add a Version::flatten() method (similar to 
   * what getObjects() does) that consolidates the objects in an internal workingSet container
   */
  boost::ptr_set<VersionedObject> Version::getObjects() const
  {
    boost::ptr_set<VersionedObject> objects = parentObjects;

    objects.insert(addedObjects);

    boost::ptr_set<VersionedObject>::iterator it;

    for (it = removedObjects.begin(); it != removedObjects.end(); ++it)
      objects.erase(*it);

    return objects;
  }

  bool Version::operator== (const Version& other) const
  {
    if (this->getId() == other.getId())
      return true;

    // TODO: check parent and objects too

    return false;
  }

  bool Version::operator!= (const Version& other) const
  {
    return !(*this == other);
  }

  std::ostream& Version::dump(std::ostream& o) const
  {
    if (*this == Version::NOT_FOUND)
      o << "NOT_FOUND";
    else if (*this == Version::ERROR)
      o << "ERROR";
    else
      o << getId();

    return o;
  }

  std::ostream& operator<<(std::ostream& o, const Version& v)
  {
    return v.dump(o);
  }
}
