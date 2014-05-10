#include "versos/objdb/memobjdb.h"

#include "versos/version.h"
#include "versos/repository.h"
#include "versos/obj/object.h"

namespace versos
{
  MemObjDB::MemObjDB(const std::string&, const Options&)
  {
  }

  MemObjDB::~MemObjDB()
  {
  }

  void MemObjDB::create(const Version&, const Version&, const std::string&) throw (VersosException)
  {
  }

  void MemObjDB::commit(const Version&, const std::string&) throw (VersosException)
  {
  }

  void MemObjDB::remove(const Version&, const std::string&) throw (VersosException)
  {
  }

  boost::any MemObjDB::exec(
      const Version&,
      const std::string&,
      const std::string&,
      const std::vector<std::string>&,
      const std::type_info&)
    throw (VersosException)
  {
    throw VersosException("in memory doesn't have active capabilities");
  }

  void MemObjDB::set(const Version& v, const std::string& oid, const void* value, const std::type_info&)
    throw (VersosException)
  {
    std::string key = oid + "_" + v.getId();
    objects[key] = value;
  }

  boost::any MemObjDB::get(const Version& v, const std::string& oid, const std::type_info&)
    throw (VersosException)
  {
    std::string key = oid + "_" + v.getId();
    std::map<std::string, boost::any>::iterator found = objects.find(key);

    if (found == objects.end())
      throw VersosException("Object " + oid + " for version " + v.getId() + " not in DB");

    return found->second;
  }
}
