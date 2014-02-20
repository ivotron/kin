#include "versos/coordination/mpicoordinator.h"

#include "versos/refdb/refdb.h"

#include <boost/ptr_container/ptr_set.hpp>

#include <vector>

namespace versos
{
  MpiCoordinator::MpiCoordinator() :
    SingleClientCoordinator(RefDB::NONE), comm(MPI_COMM_NULL), leaderRank(-1)
  {
  }

  MpiCoordinator::MpiCoordinator(MPI_Comm comm, int leaderRank, RefDB& refdb, const std::string& msg) :
    SingleClientCoordinator(refdb), comm(comm), leaderRank(leaderRank)
  {
    if(MPI_Comm_rank(comm, &myRank))
      throw "Error getting MPI rank";
    this->msg = msg;
  }

  MpiCoordinator::~MpiCoordinator()
  {
  }

  int MpiCoordinator::getHeadId(std::string& id)
  {
    if (myRank == leaderRank && SingleClientCoordinator::getHeadId(id))
        MPI_Abort(comm, -1);

    std::vector<char> writable(id.size() + 1);
    std::copy(id.begin(), id.end(), writable.begin());

    if (MPI_Bcast((void *)&writable[0], 20, MPI_CHAR, leaderRank, comm))
      MPI_Abort(comm, -1);

    id = &writable[0];

    return 0;
  }

  const Version& MpiCoordinator::checkout(const std::string& id)
  {
    return SingleClientCoordinator::checkout(id);
  }

  Version& MpiCoordinator::create(const Version& parent)
  {
    return SingleClientCoordinator::create(parent);
  }

  int MpiCoordinator::add(const Version& v, VersionedObject& o)
  {
    return SingleClientCoordinator::add(v, o);
  }

  int MpiCoordinator::remove(const Version& v, VersionedObject& o)
  {
    return SingleClientCoordinator::remove(v, o);
  }

  int MpiCoordinator::commit(const Version& v)
  {
    if (refdb.commit(v))
      return -1;

    if (MPI_Barrier(comm))
      return -2;

    return 0;
  }

  int MpiCoordinator::initRepository()
  {
    return refdb.init();

    return 0;
  }

  bool MpiCoordinator::isRepositoryEmpty()
  {
    int isEmpty;

    if (myRank == leaderRank)
        if(refdb.isEmpty())
          isEmpty = 0;
        else
          isEmpty = 1;

    if (MPI_Bcast(&isEmpty, 1, MPI_INT, leaderRank, comm))
      MPI_Abort(comm, -1);

    return isEmpty == 0;
  }

  Coordinator* MpiCoordinator::clone() const
  {
    return new MpiCoordinator(comm, leaderRank, refdb, msg);
  }
}
