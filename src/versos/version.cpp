#include "versos/version.h"

#include "versos/coordination/coordinator.h"

namespace versos
{
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
      const std::string& id, std::string parentId, const std::set<std::string>& objects) :
    id(id), parentId(parentId), status(COMMITTED), addedObjects(objects)
  {
  }

  Version::~Version()
  {
  }

  void Version::add(const std::set<std::string>& o) throw (VersosException)
  {
    if (!isOK())
      throw VersosException("can't add to version with NONE status");

    if (isCommitted())
      throw VersosException("version " + getId() + " already committed");

    std::set<std::string>::iterator it;

    for (it = o.begin(); it != o.end(); ++it)
      add(*it);
  }

  void Version::remove(const std::set<std::string>& o) throw (VersosException)
  {
    if (!isOK())
      throw VersosException("can't add to version with NONE status");

    if (isCommitted())
      throw VersosException("version " + getId() + " already committed");

    std::set<std::string>::iterator it;

    for (it = o.begin(); it != o.end(); ++it)
      remove(*it);
  }

  void Version::add(const std::string& o) throw (VersosException)
  {
    if (!isOK())
      throw VersosException("can't add to version with NONE status");

    if (isCommitted())
      throw VersosException("version " + getId() + " already committed");

    if (contains(o))
      throw VersosException("version " + getId() + " already contains object " +  o);

    addedObjects.insert(o);
  }

  void Version::remove(const std::string& o) throw (VersosException)
  {
    if (!isOK())
      throw VersosException("can't remove from version with NONE status");

    if (isCommitted())
      throw VersosException("version " + getId() + " already committed");

    // TODO: check if user has written to the object, in which case we should fail (or not, based on a knob)

    if (!contains(o))
      throw VersosException("version " + getId() + " doesn't contain object " +  o);

    removedObjects.insert(o);
  }

  bool Version::isOK() const
  {
    return (*this != Version::NOT_FOUND);
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

  bool Version::contains(const std::string& o) const
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

  const std::set<std::string>& Version::getParents() const
  {
    return parentObjects;
  }

  const std::set<std::string>& Version::getAdded() const
  {
    return addedObjects;
  }

  const std::set<std::string>& Version::getRemoved() const
  {
    return removedObjects;
  }

  /*
   * TODO: the working set is maintained in parent's/added/removed sets of objects. For syncMode == 
   * at-each-add/remove, there's significant overhead if too many objects are added/removed from a transaction 
   * since the same objects are being resent every time. We can add a Version::flatten() method (similar to 
   * what getObjects() does) that consolidates the objects in an internal workingSet container
   */
  std::set<std::string> Version::getObjects() const
  {
    std::set<std::string> objects = parentObjects;

    objects.insert(addedObjects.begin(), addedObjects.end());

    std::set<std::string>::iterator it;

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
    else
      o << getId();

    return o;
  }

  std::ostream& operator<<(std::ostream& o, const Version& v)
  {
    return v.dump(o);
  }
}
