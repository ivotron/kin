#include "versos/objectversioning/memversionedobject.h"

#include "versos/version.h"

namespace versos
{
  MemVersionedObject::MemVersionedObject(const std::string& repositoryName, const std::string& baseName)
    : VersionedObject("in-mem", repositoryName, baseName)
  {
  }

  MemVersionedObject::~MemVersionedObject()
  {
  }

  int MemVersionedObject::create(const Version& p, const Version& c)
  {
    if (!isVersionOK(p) || !isVersionOK(c))
      return -1;

    return 0;
  }

  int MemVersionedObject::commit(const Version&)
  {
    return 0;
  }

  int MemVersionedObject::remove(const Version& v)
  {
    if (!isVersionOK(v))
      return -1;

    if (values.erase(v.getId()) != 1)
      return -1;
    return 0;
  }

  int MemVersionedObject::write(const Version& v, const std::string& value)
  {
    if (!isVersionOK(v))
      return -1;

    if (v.getStatus() == Version::COMMITTED)
      return -2;

    std::string oid = getId(v);

    if (oid.empty())
      return -3;

    values[oid] = value;

    return 0;
  }

  int MemVersionedObject::read(const Version& v, std::string& value)
  {
    if (!isVersionOK(v))
      return -1;

    if (values.find(getId(v)) == values.end())
      return -2;

    std::string oid = getId(v);

    if (oid.empty())
      return -3;

    value = values[oid];

    return 0;
  }

  VersionedObject* MemVersionedObject::do_clone() const
  {
    return new MemVersionedObject(repositoryName, baseName);
  }
}
