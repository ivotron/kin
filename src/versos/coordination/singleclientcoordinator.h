/**
 * coordinates a single client.
 */

#include "versos/coordination/coordinator.h"

#include "versos/options.h"
#include "versos/refdb/refdb.h"
#include "versos/objdb/objdb.h"

#ifndef SINGLE_CLIENT_COORDINATOR_H
#define SINGLE_CLIENT_COORDINATOR_H

namespace versos
{
  class Version;
  class Object;

  class SingleClientCoordinator : public Coordinator
  {
  protected:
    RefDB& refdb;
    ObjDB& objdb;
    std::string hashSeed;
    Options::ClientSync::Mode syncMode;
  public:
    SingleClientCoordinator(RefDB& refdb, ObjDB& objdb, const Options& o);
    ~SingleClientCoordinator();

    // inherited
    std::string getHeadId() throw (VersosException);
    const Version& checkout(const std::string& id) throw (VersosException);
    Version& create(const Version& parent) throw (VersosException);
    void add(Version& v, Object& o) throw (VersosException);
    void add(Version& v, const std::string& o) throw (VersosException);
    void remove(Version& v, Object& o) throw (VersosException);
    void remove(Version& v, const std::string& oid) throw (VersosException);
    int commit(Version& v) throw (VersosException);
    void makeHEAD(const Version& v) throw (VersosException);
    void initRepository() throw (VersosException);
    void openMetaDB() const throw (VersosException);
    bool isRepositoryEmpty();
    void shutdown() throw (VersosException);

    // added
    Version& create(const Version& parent, RefDB::LockType lockMode, const std::string& lockKey)
      throw (VersosException);
  };
}
#endif
