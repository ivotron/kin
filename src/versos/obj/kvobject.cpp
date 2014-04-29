#include "versos/obj/kvobject.h"

#include "versos/version.h"

namespace versos
{

KVObject::~KVObject()
{
}

KVObject::KVObject(const std::string& oid, const std::string& value) : Object(oid), value(value)
{
  boost::serialization::void_cast_register<KVObject, Object>();
}

Object* KVObject::do_clone() const
{
  return new KVObject(oid, value);
}

void KVObject::put(const std::string& value) throw (VersosException)
{
  this->value = value;
}

std::string KVObject::get() throw (VersosException)
{
  return value;
}

}
