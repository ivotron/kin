#ifndef OBJDB_H
#define OBJDB_H

#include "versos/versosexception.h"

#include <typeinfo>

namespace versos
{

class Version;
class Object;

class ObjDB
{
public:
  /**
   * creates an object based on the given parent. From a high-level point of view, this signals the 
   * beginning of writes to the given object.
   *
   * We can safely assume that the versions are safe to be operated on (i.e. it's not a Version::NOT_FOUND, 
   * etc..), since @c Coordinator should have done this check already.
   */
  virtual void create(
      const Version& parent, const Version& child, const std::string& oid) throw (VersosException) = 0;

  /**
   * marks the object as committed. From a high-level point of view, this signals the end of writes to the 
   * given object. Future attempts to write to the object will fail.
   *
   * We can safely assume that the version is safe to commit (i.e. it's not a Version::NOT_FOUND, etc..), 
   * since @c Repository has already done this check.
   */
  virtual void commit(const Version& v, const std::string& oid) throw (VersosException) = 0;

  /**
   * removes the given version of this object.
   *
   * We can safely assume that the version is safe to be removed (i.e. it's not a Version::NOT_FOUND, 
   * etc..), since @c Repository has already done this check.
   */
  virtual void remove(const Version& v, const std::string& oid) throw (VersosException) = 0;

  /**
   * reads an object from the db.
   */
  template<class T> Object* get(const Version& v, const std::string& oid) throw (VersosException)
  {
    return get(v, oid, typeid(T));
  }

  virtual void set(const Version& v, const Object& o) throw (VersosException) = 0;

protected:

  /**
   * implementation-specific.
   */
  virtual Object* get(const Version& v, const std::string& oid, const std::type_info& i) throw 
    (VersosException) = 0;
};

}
#endif
