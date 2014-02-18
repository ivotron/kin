/**
 * coordinates a single client.
 */

#ifndef SINGLE_CLIENT_COORDINATOR_H
#define SINGLE_CLIENT_COORDINATOR_H

#include "versos/coordination/coordinator.h"
#include "versos/refdb/refdb.h"

namespace versos
{
  class SingleClientCoordinator : public Coordinator
  {
  private:
    RefDB& refdb;

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
    int init();
    Coordinator* clone() const;
  private:
    std::string getMetadataObjectName();
  };
}
#endif
