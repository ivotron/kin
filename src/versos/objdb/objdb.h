#ifndef OBJDB_H
#define OBJDB_H

#include "versos/versosexception.h"
#include "versos/version.h"

#include <typeinfo>
#include <vector>

#include <boost/any.hpp>

namespace versos
{

class Object;

/**
 * ObjDB::set and ObjDB::get are part of the API since we assume they're "universal". For other 
 * backend-specific ones, the ObjDB::exec method is provided.
 */
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
   * sets the value of an object to the given one.
   */
  template<class T> void set(const Version& v, const std::string& oid, const T& value) throw (VersosException)
  {
    set(v, oid, &v, typeid(value));
  }

  /**
   * reads an object from the db. Caller is responsible for releasing memory.
   */
  template<class T> T* get(const Version& v, const std::string& oid) throw (VersosException)
  {
    // TODO: ease memory handling for caller by either returning a std::unique_ptr or boost::shared_ptr or 
    // returning by value instead
    T* o = boost::any_cast<T*>(get(v, oid, typeid(T)));

    if (o == NULL)
      throw VersosException(
          "Object " + oid + " for version " + v.getId() + " expected type: " + typeid(*o).name() +
          " but got " + typeid(T).name());

    return o;
  }

  /**
   * executes a function on the remote object. Caller is responsible for realising memory.
   *
   * @return an instance of an object that represents the result of the function. @c NULL if the function 
   * doesn't return a value
   */
  template<class T> T* exec(
      const Version& v, const std::string& oid, const std::string& f, const std::vector<std::string>& args) 
    throw (VersosException)
  {
    // TODO: ease memory handling for caller by either returning a std::unique_ptr or boost::shared_ptr or 
    // returning by value instead
    T* r = boost::any_cast<T*>(exec(v, oid, f, args, typeid(T)));

    if (r == NULL)
      throw VersosException(
          "Object " + oid + " for version " + v.getId() + " expected type: " + typeid(T).name() +
          " but got " + typeid(*r).name());

    return r;
  }

protected:

  /**
   * backend-specific.
   */
  virtual void set(
      const Version& v, const std::string& oid, const void* value, const std::type_info& i)
    throw (VersosException) = 0;

  /**
   * backend-specific.
   */
  virtual boost::any get(
      const Version& v, const std::string& oid, const std::type_info& i) throw (VersosException) = 0;

  /**
   * backend-specific.
   */
  virtual boost::any exec(
      const Version& v,
      const std::string& oid,
      const std::string& f,
      const std::vector<std::string>& args,
      const std::type_info& i)
    throw (VersosException) = 0;
};

class None
{
};
}
#endif
