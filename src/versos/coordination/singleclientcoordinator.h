/**
 * coordinates a single client.
 */

#include "versos/coordination/coordinator.h"

#ifndef SINGLE_CLIENT_COORDINATOR_H
#define SINGLE_CLIENT_COORDINATOR_H

namespace versos
{
  class RefDB;

  class SingleClientCoordinator : public Coordinator
  {
  protected:
    RefDB& refdb;
    std::string msg;
  public:
    SingleClientCoordinator();
    SingleClientCoordinator(RefDB& refdb);
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
    Coordinator* clone() const;
  };
}
#endif
