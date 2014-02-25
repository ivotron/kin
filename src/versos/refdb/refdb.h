#ifndef REFDB_H
#define REFDB_H

#include <string>
#include <boost/shared_ptr.hpp>

namespace versos
{
  class Version;

  /**
   */
  class RefDB
  {
  protected:
    std::string repoName;
    std::string headId;

  public:
    RefDB(const std::string& repoName);
    virtual ~RefDB();
    virtual Version& create(const Version&, const std::string& msg);
    virtual const std::string& getHeadId() const;

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
     * marks the given version as committed.
     */
    virtual int commit(const Version& v) = 0;

    /**
     * retrieves the metadata of the given version id.
     */
    virtual const Version& checkout(const std::string& id) = 0;

    /**
     * removes the given version from the DB and frees memory of the @c Version object.
     */
    virtual int remove(const Version& v) = 0;

    /**
     * acquires a lock for the given version.
     */
    virtual int lock(const Version& v, int mode) = 0;

  protected:
    /**
     * takes ownership of the passed pointer.
     */
    virtual int own(boost::shared_ptr<Version> v) = 0;
  };
  // LogRefDB (log-structured db):
  //
  // 1. get the diff of object removal/deletion w.r.t. parent version
  // 2. for each added/removed object, add/remove it to the metadata object
}
#endif
