#include "versos/objectversioning/memversionedobject.h"

#include "versos/version.h"
#include "versos/repository.h"

BOOST_CLASS_EXPORT_KEY(versos::MemVersionedObject);
BOOST_CLASS_EXPORT_IMPLEMENT(versos::MemVersionedObject);

namespace versos
{
  MemVersionedObject::MemVersionedObject()
    : VersionedObject("in-mem", "noRepo", "noBaseName", VersionedObject::VERIFY_CONTAINMENT)
  {
    boost::serialization::void_cast_register<MemVersionedObject, VersionedObject>();
  }
  MemVersionedObject::MemVersionedObject(const std::string& repoName, const std::string& baseName,
      VersionedObject::ContainmentVerification check)
    : VersionedObject("in-mem", repoName, baseName, check)
  {
    boost::serialization::void_cast_register<MemVersionedObject, VersionedObject>();
  }
  MemVersionedObject::MemVersionedObject(const Repository& repo, const std::string& baseName)
    : VersionedObject("in-mem", repo, baseName, VersionedObject::VERIFY_CONTAINMENT)
  {
    boost::serialization::void_cast_register<MemVersionedObject, VersionedObject>();
  }
  MemVersionedObject::MemVersionedObject(const Repository& repo, const std::string& baseName, 
      VersionedObject::ContainmentVerification check)
    : VersionedObject("in-mem", repo, baseName, check)
  {
    boost::serialization::void_cast_register<MemVersionedObject, VersionedObject>();
  }

  MemVersionedObject::~MemVersionedObject()
  {
  }

  int MemVersionedObject::create(const Version&, const Version&)
  {
    return 0;
  }

  int MemVersionedObject::commit(const Version&)
  {
    return 0;
  }

  int MemVersionedObject::remove(const Version& v)
  {
    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    if (values.find(id) == values.end())
      ; // OK, maybe user didn't write to this object in this version,
        // but this is not our business though, layers above should handle this
    else if (values.erase(id) != 1)
      return -41;

    return 0;
  }

  int MemVersionedObject::write(const Version& v, const std::string& value)
  {
    if (v.isCommitted())
      return -42;

    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    values[id] = value;

    return 0;
  }

  int MemVersionedObject::read(const Version& v, std::string& value)
  {
    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    if (values.find(id) == values.end())
      return -43;

    value = values[id];

    return 0;
  }

  VersionedObject* MemVersionedObject::do_clone() const
  {
    return new MemVersionedObject(repoName, baseName, checkForContainment);
  }
}
