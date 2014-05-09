#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "versos/version.h"
#include "versos/options.h"
#include "versos/obj/object.h"
#include "versos/objdb/objdb.h"
#include "versos/versosexception.h"

#include <iostream>
#include <string>

//#include "versos/obj/kvobject.h"

namespace versos
{
  class Coordinator;
  class RefDB;
  class Object;

  /**
   * The main interface for checking-out/creating versions.
   *
   * **NOTE**: not thread-safe (user should instantiate another ::Repository object instead).
   */
  class Repository
  {
  private:
    std::string name;
    Coordinator* coordinator;
    RefDB* refdb;
    ObjDB* objdb;

  public:
    /**
     * creates a new versioning context for the given repository name.
     */
    Repository(const std::string& name, const Options& o);
    Repository();
    ~Repository();

    /**
     * checks whether the repository is empty
     */
    bool isEmpty() const;

    /**
     * initializes a repository.
     */
    void init() throw (VersosException);

    /**
     * checks out a given version.
     *
     * @returns Version::NOT_FOUND if no version is associated with the given ID.
     * @exception VersosException if Coordinator::checkout throws an exception.
     */
    const Version& checkout(const std::string& id) const throw (VersosException);

    // TODO: checkout() only allows access to committed versions. We can add a retrieve() call that allows to
    // get a staged versions

    /**
     * returns the latest committed version.
     */
    const Version& checkoutHEAD() const throw (VersosException);

    /**
     * creates a new staged version based on the given parent.
     */
    Version& create(const std::string& parentId) throw (VersosException);

    /**
     * creates a new staged version based on the given parent.
     */
    Version& create(const Version& parent) throw (VersosException);

    /**
     * Adds an object to the given version. Fails if version is read only.
     */
    void add(Version& v, Object& o) throw (VersosException);
    // TODO: add(Version& v, set<Object> objects);

    /**
     * removes an object.
     */
    void remove(Version& v, Object& o) throw (VersosException);

    /**
     * commits a version. TODO: currently it also updates the HEAD but this will change when we add support
     * for push(). In other words, commit() should just commit, whereas push() should take care of updating
     * HEAD and reporting about conflicts.
     */
    int commit(Version& v) throw (VersosException);

    /**
     * retrieves the value of an object from the underlying db.
     */
    template<class T> T* get(const Version& v, const std::string& oid) throw (VersosException)
    {
      //
      // TODO: ease memory handling for caller by either returning a std::unique_ptr or boost::shared_ptr or 
      // returning by value instead

      // TODO: should be using
      //
      //   dynamic_cast<T*>(objdb->get<T>(v, oid))
      //
      // but looks like I'm experiencing a GCC bug. So in the meantime we're type-checking ourselves
      Object* o = objdb->get<T>(v, oid);

      if (o != NULL && typeid(*o) != typeid(T))
        throw VersosException(
            "Object " + oid + " for version " + v.getId() + " expected type: " + typeid(*o).name() +
            " but got " + typeid(T).name());

      return (T*)(o);
    }

    /**
     * retrieves the value of an object from the underlying db.
     */
    template<class T> T* exec(
        const Version& v, const std::string& oid, const std::string& f, const std::vector<std::string>& args) 
      throw (VersosException)
    {
      //
      // TODO: ease memory handling for caller by either returning a std::unique_ptr or boost::shared_ptr or 
      // returning by value instead

      // TODO: should be using
      //
      //   dynamic_cast<T*>(objdb->get<T>(v, oid))
      //
      // but looks like I'm experiencing a GCC bug. So in the meantime we're type-checking ourselves
      Object* o = objdb->exec<T>(v, oid, f, args);

      if (o != NULL && typeid(*o) != typeid(T))
        throw VersosException(
            "Object " + oid + " for version " + v.getId() + " expected type: " + typeid(*o).name() +
            " but got " + typeid(T).name());

      return (T*)(o);
    }

    /**
     * sets an object to the underlying object db.
     */
    void set(const Version& v, const Object& o) throw (VersosException);

    /**
     */
    void close() throw (VersosException);

    /**
     */
    const std::string& getName() const;
  };
}
#endif
