#ifndef REFDB_H
#define REFDB_H

#include <string>
#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  class Version;
  class VersionedObject;

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
    const Version& checkout(const std::string& id);

    /**
     * inits a new db. Might fail if the db has been initialized previously
     */
    virtual int init();

    /**
     * opens the db.
     */
    virtual int open() = 0;

    /**
     * closes the db.
     */
    virtual int close() = 0;

    /**
     * whether the db is empty.
     */
    virtual bool isEmpty() const = 0;

    /**
     * makes the given version the head of the repo.
     */
    virtual int makeHEAD(const Version& v) = 0;

    /**
     * changes the status of a version stored at the backed to Version::COMMITTED.
     */
    virtual int commit(const Version& v) = 0;

    /**
     * modifies the version stored in the db so that it adds an object to it. While we have the guarantee that 
     * the given @c Version object has been already checked for OKness, this method might fail if the remote 
     * version being updated has been committed externally (i.e. not through an instance of this object).
     */
    virtual int add(const Version& v, const VersionedObject& o) = 0;

    /**
     * modifies the version stored in the db so that metadata of all objects of the given version are added to 
     * it. While we have the guarantee that the given @c Version object has been already checked for OKness, 
     * this method might fail if the remote version being updated has been committed externally (i.e. not 
     * through an instance of this object).
     */
    virtual int add(const Version& v, const boost::ptr_set<VersionedObject>& o) = 0;

    /**
     * modifies the version stored in the db so that metadata of all objects of the given version are added to 
     * it. While we have the guarantee that the given @c Version object has been already checked for OKness, 
     * this method might fail if the remote version being updated has been committed externally (i.e. not 
     * through an instance of this object).
     */
    virtual int remove(const Version& v, const boost::ptr_set<VersionedObject>& o) = 0;

    /**
     * modifies the version stored in the db so that it removes an object from it. While we have the guarantee 
     * that the given @c Version object has been already checked for OKness, this method might fail if the 
     * remote version being updated has been committed externally (i.e. not through an instance of this 
     * object).
     */
    virtual int remove(const Version& v, const VersionedObject& o) = 0;

    /**
     * for versions created with ::SHARED_LOCK, returns the number of locks placed on it.
     */
    virtual int getLockCount(const Version& v, const std::string& lockKey) = 0;

  protected:
    /**
     * retrieves the metadata of the given version id. Returns ::Version::NOT_FOUND if the version doesn't 
     * exist in the db.
     */
    virtual Version& get(const std::string& id) = 0;

    /**
     * adds the given version to the database and acquires an EXCLUSIVE_LOCK over the version.
     */
    int insert(Version& v);

    /**
     * adds the given version to the database. When a SHARED_LOCK is given, a lockKey should be passed too. 
     * The implementation should take ownership of the passed pointer.
     */
    virtual int insert(Version& v, LockType lock, const std::string& lockKey) = 0;
  };
}
#endif
