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
   * sets the value of an object to the given one. This method invokes the copy constructor of the passed 
   * object.
   */
  template<class T> void set(const Version& v, const std::string& oid, const T& value) throw (VersosException)
  {
    set(v, oid, value, typeid(value));
  }

  /**
   * reads an object from the db.
   */
  template<class T> T get(const Version& v, const std::string& oid) throw (VersosException)
  {
    // TODO: catch boost::bad_any_cast exception
    return boost::any_cast<T>(get(v, oid, typeid(T)));
  }

  /**
   * executes a function on the remote object.
   */
  template<class T> T exec(
      const Version& v, const std::string& oid, const std::string& f, const std::vector<std::string>& args) 
    throw (VersosException)
  {
    // TODO: catch boost::bad_any_cast exception
    return boost::any_cast<T>(exec(v, oid, f, args, typeid(T)));
  }

protected:

  /**
   * backend-specific. Passed value (as in key-value) should be passed _by value_, otherwise behavior is 
   * undefined. Thus, this method invokes the copy constructor of the passed object.
   */
  virtual void set(
      const Version& v, const std::string& oid, boost::any value, const std::type_info& i)
    throw (VersosException) = 0;

  /**
   * backend-specific. Should return by value.
   */
  virtual boost::any get(
      const Version& v, const std::string& oid, const std::type_info& i) throw (VersosException) = 0;

  /**
   * backend-specific. Should return by value.
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
