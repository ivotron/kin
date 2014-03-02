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
  private:
    Options::ClientSync::Mode syncMode;
  public:
    BackendCoordinator(RefDB& refdb);
    BackendCoordinator(RefDB& refdb, const Options& o);
    BackendCoordinator(RefDB& refdb, Options::ClientSync::Mode syncMode, const std::string& hashSeed);
    BackendCoordinator(const BackendCoordinator& copy);
    ~BackendCoordinator();

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
  private:
    void init();
  };
}
#endif

