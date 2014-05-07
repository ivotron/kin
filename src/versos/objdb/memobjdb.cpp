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

  void MemObjDB::set(const Version& v, const Object& o) throw (VersosException)
  {
    std::string key = o.getId() + "_" + v.getId();
    objects.insert(key, o.clone());
  }

  void MemObjDB::get(const Version& v, const std::string& oid, Object* &o, const std::type_info& i)
    throw (VersosException)
  {
    std::string key = oid + "_" + v.getId();
    boost::ptr_map<std::string, Object>::iterator found = objects.find(key);

    if (found == objects.end())
      throw VersosException("Object " + oid + " for version " + v.getId() + " not in DB");

    o = found->second;
  }
}
