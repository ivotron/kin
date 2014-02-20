#include "versos/version.h"

#include "versos/coordination/coordinator.h"
#include "versos/objectversioning/versionedobject.h"

namespace versos
{
  Version Version::ERROR = Version("1");
  Version Version::NOT_FOUND = Version("2");
  Version Version::PARENT_FOR_ROOT = Version("3");
  Version Version::PARENT_NOT_FOUND = Version("4");
  Version Version::PARENT_NOT_RETRIEVED = Version("5");
  Version Version::NOT_COMMITTED = Version("6");
  Version Version::PARENT_NOT_COMMITTED = Version("7");

  Version::Version(const std::string& id) :
    id(id), parentId(""), status(Version::NONE), coordinator(NULL)
  {
    if (id == "3")
      // special case for PARENT_FOR_ROOT
      status = COMMITTED;
  }

  Version::Version(std::string id, const Version& parent, Coordinator& c) :
    id(id), parentId(parent.getId()), status(STAGED), objects(parent.objects), coordinator(&c)
  {
  }

  Version::Version(std::string id, std::string parentId, const boost::ptr_set<VersionedObject>& objects, 
      Coordinator& c) :
    id(id), parentId(parentId), status(COMMITTED), objects(objects), coordinator(&c)
  {
  }

  Version::Version(const Version& copy) :
    id(copy.id), parentId(copy.parentId), status(copy.status), objects(copy.objects), 
    coordinator(copy.coordinator)
  {
  }

  Version::~Version()
  {
  }

  int Version::add(VersionedObject& o)
  {
    if (status != STAGED)
      return -1;

    if (contains(o))
      return -2;

    objects.insert(o.clone());

    if(coordinator->add(*this, o))
      return -3;

    return 0;
  }

  int Version::remove(VersionedObject& o)
  {
    if (!contains(o))
      return -1;

    if (coordinator->remove(*this, o))
      return -2;

    if (objects.erase(o) != 0)
      return -3;

    return 0;
  }

  int Version::commit()
  {
    if (status != STAGED)
      return -1;

    if (coordinator->commit(*this))
      return -2;

    status = COMMITTED;

    return 0;
  }

  const std::string& Version::getParentId() const
  {
    return parentId;
  }

  bool Version::isCommitted() const
  {
    return status == COMMITTED;
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

  const boost::ptr_set<VersionedObject>& Version::getObjects() const
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
}
