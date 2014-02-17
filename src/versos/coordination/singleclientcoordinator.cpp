#include "versos/coordination/singleclientcoordinator.h"

namespace versos
{
  SingleClientCoordinator::SingleClientCoordinator() : io(NONE), repo(Repository::NONE)
  {
  }

  SingleClientCoordinator::SingleClientCoordinator(Repository& repo, librados::IoCtx& io) :
    io(io), repo(repo)
  {
  }

  int SingleClientCoordinator::getHeadId(uint64_t& head)
  {
    librados::bufferlist b;

    //if (ioctx.getxattr(getMetadataObjectName(), bl, len, off) < 0)
      //return -1;

    // simple:
    //
    //   1. Query the "head" object which points to the highest committed id
    //   2. return
    //
    return 0;
  }

  Version& SingleClientCoordinator::checkout(uint64_t id)
  {
    // 1. get the list of object names at id
    // 2. instantiate and populate @c Version object with object list
    // 3. return
  }

  Version& SingleClientCoordinator::create(const Version& parent)
  {
    // 1. get new id for this version
    // 2. instantiate @c Version with new id
  }

  int SingleClientCoordinator::add(const Version&, VersionedObject&)
  {
    return 0;
  }

  int SingleClientCoordinator::remove(const Version& v, VersionedObject& o)
  {
    o.remove(v);
    return 0;
  }

  int SingleClientCoordinator::commit(const Version& v)
  {
    // simple:
    //
    // 1. write all the contents of v to the metadata object
    // 2. loop on the objects list and create a snapshot for each
    // 3. return
    //
    // log-structured:
    //
    // 1. get the diff of object removal/deletion w.r.t. parent version
    // 2. for each added/removed object, add/remove it to the metadata object
    // 3. go to 2-3 of simple
  }

  Coordinator* SingleClientCoordinator::clone() const
  {
    return new SingleClientCoordinator(repo, io);
  }
}
