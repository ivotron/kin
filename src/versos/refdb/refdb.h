#ifndef REFDB_H
#define REFDB_H

#include "versos/versosexception.h"

#include <set>
#include <string>

namespace versos
{
  class Version;

  /**
   */
  class RefDB
  {
  public:
    enum LockType { EXCLUSIVE_LOCK, SHARED_LOCK };

  protected:
    std::string repoName;
    std::string headId;

  public:
    RefDB(const std::string& repoName);
    virtual ~RefDB();

    /**
     */
    Version& create(const Version&, const std::string& hashSeed);

    /**
     */
    Version& create(
        const Version&, const std::string& hashSeed, LockType mode, const std::string& lockKey);

    /**
     */
    const std::string& getHeadId() const;

    /**
     * retrieves the metadata of the given version id. Returns Version::ERROR If the version is not committed; 
     * Version::NOT_FOUND if the version doesn't exist in the db.
     */
    const Version& checkout(const std::string& id) throw (VersosException);

    /**
     * inits a new db. Might fail if the db has been initialized previously
     */
    virtual void init() throw (VersosException);

    /**
     * opens the db.
     */
    virtual void open() throw (VersosException) = 0;

    /**
     * closes the db.
     */
    virtual void close() throw (VersosException) = 0;

    /**
     * whether the db is empty.
     */
    virtual bool isEmpty() const = 0;

    /**
     * makes the given version the head of the repo.
     */
    virtual void makeHEAD(const Version& v) throw (VersosException) = 0;

    /**
     * changes the status of a version stored at the backed to Version::COMMITTED.
     *
     * @return the number of locks that remain placed on the version (i.e. decreases the lock count and 
     * returns this number).
     */
    virtual int commit(const Version& v) throw (VersosException) = 0;

    /**
     * adds an object to a version.
     */
    virtual void add(const Version& v, const std::string& oid) throw (VersosException) = 0;

    /**
     * adds a set of objects to the given version.
     */
    virtual void add(const Version& v, const std::set<std::string>& oid) throw (VersosException) = 0;

    /**
     * removes an object from the given version.
     */
    virtual void remove(const Version& v, const std::string& oid) throw (VersosException) = 0;

    /**
     * removes a set of objects from the given version.
     */
    virtual void remove(const Version& v, const std::set<std::string>& oid) throw (VersosException) = 0;

    /**
     * for versions created with ::SHARED_LOCK, returns the number of locks placed on it.
     */
    virtual int getLockCount(const Version& v, const std::string& lockKey) throw (VersosException) = 0;

  protected:
    /**
     * retrieves the metadata of the given version id. Returns ::Version::NOT_FOUND if the version doesn't 
     * exist in the db.
     */
    virtual Version& get(const std::string& id) throw (VersosException) = 0;

    /**
     * adds the given version to the database and acquires an EXCLUSIVE_LOCK over the version.
     */
    void insert(Version& v) throw (VersosException);

    /**
     * adds the given version to the database. When a SHARED_LOCK is given, a lockKey should be passed too. 
     */
    virtual void insert(Version& v, LockType lock, const std::string& lockKey) throw (VersosException) = 0;
  };
}
#endif
