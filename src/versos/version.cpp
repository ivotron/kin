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

  Version::Version(const std::string& id) : id(id), parentId(""), status(Version::NONE)
  {
    if (id == "0000000000000000000000000000000000000003")
      // special case
      status = COMMITTED;
  }

  Version::Version(const std::string& id, const Version& parent) :
    id(id), parentId(parent.getId()), status(STAGED), objects(parent.objects)
  {
  }

  Version::Version(
      const std::string& id, std::string parentId, const boost::ptr_set<VersionedObject>& objects) :
    id(id), parentId(parentId), status(COMMITTED), objects(objects)
  {
  }

  Version::Version(const Version& copy) :
    id(copy.id), parentId(copy.parentId), status(copy.status), objects(copy.objects)
  {
  }

  Version::~Version()
  {
  }

  int Version::add(VersionedObject& o)
  {
    if (contains(o))
      return -12;

    objects.insert(o.clone());

    return 0;
  }

  int Version::remove(VersionedObject& o)
  {
    if (!contains(o))
      return -15;

    if (objects.erase(o) != 1)
      return -16;

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
    return objects.find(o) != objects.end();
  }

  Version::Status Version::getStatus() const
  {
    return status;
  }

  const std::string& Version::getId() const
  {
    return id;
  }

  int Version::size() const
  {
    return objects.size();
  }

  boost::ptr_set<VersionedObject>& Version::getObjects()
  {
    return objects;
  }

  bool Version::operator== (const Version& other) const
  {
    if (this->getId() == other.getId())
      return true;

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
