/**
 * in-memory db.
 */

#ifndef MEM_VERSIONED_OBJECT_H
#define MEM_VERSIONED_OBJECT_H

#include "versos/objdb/objdb.h"

#include "versos/options.h"

#include <boost/ptr_container/ptr_map.hpp>
#include <string>

namespace versos
{
class Object;

class MemObjDB : public ObjDB
{
private:
  boost::ptr_map<std::string, Object> objects;
public:
  MemObjDB(const std::string& repoName, const Options& o);
  virtual ~MemObjDB();

  void create(const Version& parent, const Version& child, const std::string& object) throw (VersosException);
  void commit(const Version& v, const std::string& object) throw (VersosException);
  void remove(const Version& v, const std::string& object) throw (VersosException);
  void set(const Version& v, const Object& o) throw (VersosException);
protected:
  void get(const Version& v, const std::string& id, Object* &o, const std::type_info& i)
    throw (VersosException);
};

}
#endif
