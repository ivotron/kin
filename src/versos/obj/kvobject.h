#ifndef KV_OBJECT_H
#define KV_OBJECT_H

#include "versos/obj/object.h"

#include <map>
#include <string>

namespace versos
{

class KVObject : public Object
{
private:
  std::string value;
  virtual Object* do_clone() const;
public:
  KVObject(const std::string& oid, const std::string& value);
  virtual ~KVObject();
  void put(const std::string& value) throw (VersosException);
  std::string get() throw (VersosException);
};

}
#endif
