#ifndef REFDB_H
#define REFDB_H

#include "versos/version.h"
#include "versos/coordination/coordinator.h"
#include "versos/objectversioning/versionedobject.h"

#include <string>
#include <boost/ptr_container/ptr_set.hpp>

namespace versos
{
  /**
   */
  class RefDB
  {
  private:
    std::string noneOrValid;
    RefDB(std::string);

  public:
    static RefDB NONE;

    RefDB();
    virtual ~RefDB();
    virtual int init();
    virtual bool isEmpty() const;
    virtual const std::string& getHeadId() const;
    virtual const Version& checkout(const std::string&);
    virtual Version& create(const Version&, Coordinator&, const std::string&);

    virtual int remove(const Version&);
    /**
     * lock a revision. modes = EX or SHARED
     */
    virtual int lock(const Version&, int );
    virtual int commit(const Version&);
    virtual RefDB* clone();
  };

    // log-structured:
    //
    // 1. get the diff of object removal/deletion w.r.t. parent version
    // 2. for each added/removed object, add/remove it to the metadata object

}
#endif
