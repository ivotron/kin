#ifndef REFDB_H
#define REFDB_H

#include <string>
#include <boost/shared_ptr.hpp>

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
    // TODO: enum CommitStatusCheck { DONT_CHECK_FOR_COMMIT_STATUS, CHECK_FOR_COMMIT_STATUS };

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
     * inits a new db. Might fail if the db has been initialized previously
     */
    int init();

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
     * marks the given version as committed.
     */
    virtual int commit(const Version& v) = 0;

    // TODO: int get(Version& v, set<Obj> objects);

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
    virtual int addAll(const Version& v) = 0;

    /**
     * modifies the version stored in the db so that it removes an object from it. While we have the guarantee 
     * that the given @c Version object has been already checked for OKness, this method might fail if the 
     * remote version being updated has been committed externally (i.e. not through an instance of this 
     * object).
     */
    virtual int remove(const Version& v, const VersionedObject& o) = 0;

    /**
     * retrieves the metadata of the given version id.
     */
    virtual const Version& checkout(const std::string& id) = 0;

    /**
     * removes the given version from the DB and frees memory of the @c Version object.
     */
    virtual int remove(const Version& v) = 0;

  protected:
    /**
     * adds the given version to the database and acquires an EXCLUSIVE_LOCK over the version.
     */
    int own(boost::shared_ptr<Version> v);

    /**
     * adds the given version to the database. When a SHARED_LOCK is given, a lockKey should be passed too. 
     * The implementation should take ownership of the passed pointer.
     */
    virtual int own(boost::shared_ptr<Version> v, LockType lock, const std::string& lockKey) = 0;
  };
}
#endif
