/**
 * coordinates a single client.
 */

#ifndef SINGLE_CLIENT_COORDINATOR_H
#define SINGLE_CLIENT_COORDINATOR_H

#include "versos/repository.h"
#include "versos/coordination/coordinator.h"

#include "rados/librados.hpp"

namespace versos
{
  class SingleClientCoordinator : public Coordinator
  {
  private:
    static librados::IoCtx NONE;
    librados::IoCtx& io;
    Repository& repo;

  public:
    SingleClientCoordinator();
    SingleClientCoordinator(Repository& repo, librados::IoCtx& io);
    ~SingleClientCoordinator();

    // inherited
    int getHeadId(uint64_t& id);
    Version& checkout(uint64_t id);
    Version& create(const Version& parent);
    int add(const Version& v, VersionedObject& o);
    int remove(const Version& v, VersionedObject& o);
    int commit(const Version& v);
    Coordinator* clone() const;
  };
}
#endif
