#ifndef OBJDB_H
#define OBJDB_H

#include "versos/versosexception.h"

#include <typeinfo>
#include <vector>

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
   */
  virtual void create(
      const Version& parent, const Version& child, const std::string& oid) throw (VersosException) = 0;

  /**
   * marks the object as committed. From a high-level point of view, this signals the end of writes to the 
   * given object. Future attempts to write to the object will fail.
   */
  virtual void commit(const Version& v, const std::string& oid) throw (VersosException) = 0;

  /**
   * removes the given version of this object.
   */
  virtual void remove(const Version& v, const std::string& oid) throw (VersosException) = 0;

  /**
   * reads an object from the db. Caller is responsible of memory.
   */
  template<class T> Object* get(const Version& v, const std::string& oid) throw (VersosException)
  {
    return get(v, oid, typeid(T));
  }

  /**
   * executes a function on the remote object. Caller is responsible of memory. If the function returns an 
   * object, it should be specified in the template argument.
   *
   * @return an instance of an object that represents the result of the function. @c NULL if the function 
   * doesn't return a value
   */
  template<class T> Object* exec(
      const Version& v, const std::string& oid, const std::string& f, const std::vector<std::string>& args) 
    throw (VersosException)
  {
    return exec(v, oid, f, args, typeid(T));
  }

  virtual void set(const Version& v, const Object& o) throw (VersosException) = 0;

protected:

  /**
   * implementation-specific.
   */
  virtual Object* get(
      const Version& v, const std::string& oid, const std::type_info& i) throw (VersosException) = 0;

  /**
   * implementation-specific.
   */
  virtual Object* exec(
      const Version& v,
      const std::string& oid,
      const std::string& f,
      const std::vector<std::string>& args,
      const std::type_info& i)
    throw (VersosException) = 0;
};

}
#endif
