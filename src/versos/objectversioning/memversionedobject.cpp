#include "versos/objectversioning/memversionedobject.h"

#include "versos/version.h"
#include "versos/repository.h"

BOOST_CLASS_EXPORT_KEY(versos::MemVersionedObject);
BOOST_CLASS_EXPORT_IMPLEMENT(versos::MemVersionedObject);

namespace versos
{
  MemVersionedObject::MemVersionedObject()
    : VersionedObject("in-mem", "noRepo", "noBaseName")
  {
    boost::serialization::void_cast_register<MemVersionedObject, VersionedObject>();
  }
  MemVersionedObject::MemVersionedObject(const std::string& repoName, const std::string& baseName)
    : VersionedObject("in-mem", repoName, baseName)
  {
    boost::serialization::void_cast_register<MemVersionedObject, VersionedObject>();
  }
  MemVersionedObject::MemVersionedObject(const Repository& repo, const std::string& baseName)
    : VersionedObject("in-mem", repo, baseName)
  {
    boost::serialization::void_cast_register<MemVersionedObject, VersionedObject>();
  }

  MemVersionedObject::~MemVersionedObject()
  {
  }

  void MemVersionedObject::create(const Version&, const Version&) throw (VersosException)
  {
  }

  void MemVersionedObject::commit(const Version&) throw (VersosException)
  {
  }

  void MemVersionedObject::remove(const Version& v) throw (VersosException)
  {
    std::string id = getId(v);

    if (values.find(id) == values.end())
      ; // OK, maybe user didn't write to this object in this version,
        // but this is not our business though, layers above should handle this
    else if (values.erase(id) != 1)
      throw VersosException("Object not removed");
  }

  void MemVersionedObject::put(const Version& v, const std::string& value) throw (VersosException)
  {
    if (v.isCommitted())
      throw VersosException("Version already committed");

    values[getId(v)] = value;
  }

  std::string MemVersionedObject::get(const Version& v) throw (VersosException)
  {
    std::string id = getId(v);

    if (values.find(id) == values.end())
      throw VersosException("object not found");

    return values[id];
  }

  VersionedObject* MemVersionedObject::do_clone() const
  {
    return new MemVersionedObject(repoName, baseName);
  }
}
