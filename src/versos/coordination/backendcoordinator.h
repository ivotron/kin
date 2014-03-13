/**
 * coordinates a single client.
 */

#include "versos/coordination/singleclientcoordinator.h"

#include "versos/options.h"

#ifndef BACKEND_COORDINATOR_H
#define BACKEND_COORDINATOR_H

namespace versos
{
  class RefDB;
  class Version;

  class BackendCoordinator : public SingleClientCoordinator
  {
  public:
    BackendCoordinator(RefDB& refdb, const Options& o);
    ~BackendCoordinator();

    // inherited
    Version& create(const Version& parent);
    int makeHEAD(const Version& v);
    int commit(Version& v);
  };
}
#endif

