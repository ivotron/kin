/**
 * coordinates a single client.
 */

#include "versos/coordination/coordinator.h"

#include "versos/options.h"

#ifndef SINGLE_CLIENT_COORDINATOR_H
#define SINGLE_CLIENT_COORDINATOR_H

namespace versos
{
  class RefDB;
  class Version;

  class SingleClientCoordinator : public Coordinator
  {
  protected:
    RefDB& refdb;
    std::string hashSeed;
  public:
    SingleClientCoordinator(const SingleClientCoordinator& copy);
    SingleClientCoordinator(RefDB& refdb, const Options& o);
    SingleClientCoordinator(RefDB& refdb);
    SingleClientCoordinator(RefDB& refdb, const std::string& hashSeed);
    ~SingleClientCoordinator();

    // inherited
    int getHeadId(std::string& id);
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent);
    int add(Version& v, VersionedObject& o);
    int remove(Version& v, VersionedObject& o);
    int commit(Version& v);
    int initRepository();
    bool isRepositoryEmpty();
    int shutdown();
  };
}
#endif
