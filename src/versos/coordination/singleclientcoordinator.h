/**
 * coordinates a single client.
 */

#include "versos/coordination/coordinator.h"

#include "versos/options.h"
#include "versos/refdb/refdb.h"

#ifndef SINGLE_CLIENT_COORDINATOR_H
#define SINGLE_CLIENT_COORDINATOR_H

namespace versos
{
  class Version;

  class SingleClientCoordinator : public Coordinator
  {
  protected:
    RefDB& refdb;
    std::string hashSeed;
    Options::ClientSync::Mode syncMode;
  public:
    SingleClientCoordinator(RefDB& refdb, const Options& o);
    ~SingleClientCoordinator();

    // inherited
    std::string getHeadId() throw (VersosException);
    const Version& checkout(const std::string& id) throw (VersosException);
    Version& create(const Version& parent) throw (VersosException);
    void add(Version& v, VersionedObject& o) throw (VersosException);
    void remove(Version& v, VersionedObject& o) throw (VersosException);
    int commit(Version& v) throw (VersosException);
    void makeHEAD(const Version& v) throw (VersosException);
    void initRepository() throw (VersosException);
    bool isRepositoryEmpty();
    void shutdown() throw (VersosException);

    // added
    Version& create(const Version& parent, RefDB::LockType lockMode, const std::string& lockKey)
      throw (VersosException);
  };
}
#endif
