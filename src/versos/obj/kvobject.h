#ifndef KV_OBJECT_H
#define KV_OBJECT_H

#include "versos/obj/object.h"

#include <map>
#include <string>

namespace versos
{

/**
 * Sample implementation of an object. This is really redundant since one can obtain the value directly 
 * through ::ObjDB::get, which every backend supports.
 */
class KVObject : public Object
{
private:
  std::string value;
public:
  KVObject(const std::string& oid, const std::string& value);
  virtual ~KVObject();
  void put(const std::string& value) throw (VersosException);
  std::string get() throw (VersosException);
  const std::list<std::pair<int, int> > getModifiedOffsets() const throw (VersosException);
  const std::string serialize() const throw (VersosException);
};

}
#endif
