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
    std::string just;
  public:
    static RefDB NONE;

    virtual ~RefDB() {}
    virtual int init() {return 0;}
    virtual const std::string& getHeadId() const {return just;}
    virtual const Version& checkout(const std::string&) {return Version::ERROR;}
    virtual Version& create(const Version&, Coordinator&) {return Version::ERROR;}

    /**
     * lock a revision. modes = EX or SHARED
     */
    virtual int lock(const Version&, int ) {return 0;}
    virtual int commit(const Version&) {return 0;}
  };

    // log-structured:
    //
    // 1. get the diff of object removal/deletion w.r.t. parent version
    // 2. for each added/removed object, add/remove it to the metadata object

}
#endif
