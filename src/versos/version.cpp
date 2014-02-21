#include "versos/version.h"

#include "versos/coordination/coordinator.h"
#include "versos/objectversioning/versionedobject.h"

namespace versos
{
  Version Version::ERROR = Version("1");
  Version Version::NOT_FOUND = Version("2");
  Version Version::PARENT_FOR_ROOT = Version("3");

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
    if (!isOK())
      return -10;

    if (isCommitted())
      return -11;

    if (contains(o))
      return -12;

    objects.insert(o.clone());

    int ret = coordinator->add(*this, o);

    if(ret)
      return ret;

    return 0;
  }

  int Version::remove(VersionedObject& o)
  {
    if (!isOK())
      return -13;

    if (isCommitted())
      return -14;

    if (!contains(o))
      return -15;

    // TODO: check if user has written to the object, in which case we should fail (or not, based on a knob)

    int ret = coordinator->remove(*this, o);

    if (ret)
      return ret;

    if (objects.erase(o) != 1)
      return -16;

    return 0;
  }

  int Version::commit()
  {
    if (!isOK())
      return -17;

    if (isCommitted())
      return -18;

    int ret = coordinator->commit(*this);

    if (ret)
      return ret;

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
