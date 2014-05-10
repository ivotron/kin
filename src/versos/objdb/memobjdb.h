/**
 * in-memory db.
 */

#ifndef MEM_VERSIONED_OBJECT_H
#define MEM_VERSIONED_OBJECT_H

#include "versos/objdb/objdb.h"
#include "versos/options.h"

#include <map>
#include <string>

namespace versos
{
class Object;

class MemObjDB : public ObjDB
{
private:
  std::map<std::string, boost::any> objects;
public:
  MemObjDB(const std::string& repoName, const Options& o);
  virtual ~MemObjDB();

  void create(const Version& parent, const Version& child, const std::string& object) throw (VersosException);
  void commit(const Version& v, const std::string& object) throw (VersosException);
  void remove(const Version& v, const std::string& object) throw (VersosException);
protected:
  boost::any exec(
      const Version& v,
      const std::string& oid,
      const std::string& f,
      const std::vector<std::string>& args,
      const std::type_info& i)
    throw (VersosException);
  boost::any get(const Version& v, const std::string& id, const std::type_info& i)
    throw (VersosException);
  void set(const Version& v, const std::string& oid, const void* value, const std::type_info& i)
    throw (VersosException);
};

}
#endif
