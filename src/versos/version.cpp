#include "versos/version.h"

namespace versos
{
  Version Version::NOT_FOUND = Version(Version::NONE, 0);
  Version Version::ERROR = Version(Version::NONE, 1);
  Version Version::PARENT_FOR_ROOT = Version(Version::NONE, 2);
  Version Version::NO_VERSION = Version(Version::NONE, 3);

  Version::Version(Status status, uint64_t id) :
    id(id), parent(Version::NO_VERSION), status(status), coordinator(NULL)
  {}

  Version::Version(uint64_t id, const Version& parent, Coordinator& c) :
    id(id), parent(parent), status(STAGED), objects(parent.objects), coordinator(&c)
  {
  }

  Version::Version(const Version& copy) :
    id(copy.id), parent(copy.parent), status(copy.status), objects(copy.objects), 
    coordinator(copy.coordinator)
  {
  }

  Version::Version() : id(3), parent(Version::NO_VERSION), status(NONE), coordinator(NULL)
  {
  }

  int Version::add(VersionedObject& o)
  {
    if (status != STAGED)
      return -1;

    if (contains(o))
      return -2;

    if(coordinator->add(*this, o) < 0)
      return -3;

    objects.insert(o.clone());

    return 0;
  }

  struct compare {
    compare(const VersionedObject* ptr) : ptr(ptr) {}
    bool operator () (const VersionedObject& other) const { return &other == ptr; }
    const VersionedObject* ptr;
  };

  int Version::remove(VersionedObject& o)
  {
    if (!contains(o))
      return -1;

    if (coordinator->remove(*this, o) < 0)
      return -2;

    objects.erase(std::find_if(objects.begin(), objects.end(), compare(&o)));

    return 0;
  }

  int Version::commit()
  {
    if (status != STAGED)
      return -1;

    if (coordinator->commit(*this) < 0)
      return -2;

    status = COMMITTED;

    return 0;
  }

  bool Version::isCommitted() const
  {
    return status == COMMITTED;
  }

  bool Version::contains(const VersionedObject& o) const
  {
    return std::find_if(objects.begin(), objects.end(), compare(&o)) != objects.end();
  }

  uint64_t Version::getStatus() const
  {
    return status;
  }

  uint64_t Version::getId() const
  {
    return id;
  }

  bool Version::operator== (const Version& other) const
  {
    if (this->getStatus() == other.getStatus() &&
        this->getId() == other.getId())
      return true;

    return false;
  }
}
