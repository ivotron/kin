/**
 * coordinates a single client.
 */

#include "versos/coordination/coordinator.h"

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
    std::string msg;
  public:
    SingleClientCoordinator(const SingleClientCoordinator& copy);
    SingleClientCoordinator(RefDB& refdb);
    SingleClientCoordinator(RefDB& refdb, const std::string& msg);
    ~SingleClientCoordinator();

    // inherited
    int getHeadId(std::string& id);
    const Version& checkout(const std::string& id);
    Version& create(const Version& parent);
    int add(const Version& v, VersionedObject& o);
    int remove(const Version& v, VersionedObject& o);
    int commit(const Version& v);
    int initRepository();
    bool isRepositoryEmpty();
    int shutdown();
  };
}
#endif
