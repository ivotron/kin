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
    void add(Version& v, const std::string& oid) throw (VersosException);
    // TODO: add(Version& v, set<Object> objects);

    /**
     * removes an object.
     */
    void remove(Version& v, Object& o) throw (VersosException);
    void remove(Version& v, const std::string& oid) throw (VersosException);

    /**
     * commits a version. TODO: currently it also updates the HEAD but this will change when we add support
     * for push(). In other words, commit() should just commit, whereas push() should take care of updating
     * HEAD and reporting about conflicts.
     */
    int commit(Version& v) throw (VersosException);

    /**
     * sets an object to the underlying object db.
     */
    template<class T> void set(const Version& v, const std::string& oid, const T& value)
      throw (VersosException)
    {
      objdb->set<T>(v, oid, value);
    }

    /**
     * retrieves the value of an object from the underlying db.
     */
    template<class T> T get(const Version& v, const std::string& oid) throw (VersosException)
    {
      return objdb->get<T>(v, oid);
    }

    /**
     * invokes a backend method for the given object.
     */
    template<class T> T exec(
        const Version& v, const std::string& oid, const std::string& f, const std::vector<std::string>& args) 
      throw (VersosException)
    {
      return objdb->exec<T>(v, oid, f, args);
    }

    /**
     * convenience methods
     */
    template<class T> T exec(
        const Version& v, const std::string& oid, const std::string& f, const std::string& arg)
      throw (VersosException)
    {
      std::vector<std::string> args;
      args.push_back(arg);
      return objdb->exec<T>(v, oid, f, args);
    }
    template<class T> T exec(
        const Version& v, const Object& o, const std::string& f, const std::string& arg)
      throw (VersosException)
    {
      std::vector<std::string> args;
      args.push_back(arg);
      return objdb->exec<T>(v, o.getId(), f, args);
    }

    /**
     */
    void close() throw (VersosException);

    /**
     */
    const std::string& getName() const;
  };
}
#endif
