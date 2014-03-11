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
    int getHeadId(std::string& id);
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent);
    int add(Version& v, VersionedObject& o);
    int remove(Version& v, VersionedObject& o);
    int commit(Version& v);
    int makeHEAD(const Version& v);
    int initRepository();
    bool isRepositoryEmpty();
    int shutdown();

    // added
    Version& create(const Version& parent, RefDB::LockType lockMode, const std::string& lockKey);
  };
}
#endif
