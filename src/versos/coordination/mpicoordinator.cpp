#include "versos/coordination/mpicoordinator.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"
#include "versos/refdb/refdb.h"

#include <sstream>
#include <stdexcept>
#include <vector>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/ptr_container/ptr_set.hpp>
#include <boost/ptr_container/serialize_ptr_set.hpp>
#include <boost/serialization/serialization.hpp>

/* high-level description of the implementation:
 *
 * Every rank has an instance of an in-memory meta DB (localRefDB) which is kept synchronized with the actual 
 * refdb at the backend. There is one leader rank that contacts the refdb at the backend and propagates the 
 * info to other ranks. The points of synchronization are:
 *
 *   1. coordinator instantiation: the local metadb's are synchronized to determine the HEAD and its contents.
 *   2. A new version is created
 *   3. A version is checked out
 *   4. A version is committed (only if syncMode == Options::ClientSync::AT_EACH_COMMIT)
 *   5. An object is removed/added (only if syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
 *
 * TODO: currently, the NONE syncMode will actually store metadata about the objects added to the leader rank. 
 * This is because the leader's local in-memory db is not used at all, instead it relies on using the 
 * SingleClientCoordinator's one. Conversely, other-than-leader ranks don't use the SingleClientCoordinator's 
 * memdb at all (because only the leader contacts it). So in order to resolve this, we have to have the leader 
 * use its in-memory db just as the other ranks do.
 *
 * TODO: regardless of the above, currently there is no way of identifying conflicts w.r.t. distinct 
 * syncMode's used and the actual contents of a version. For example, an instance of the mpi coordinator might 
 * have operated in Options::ClientSync::NONE mode and committed some versions, thus no object-containment 
 * information would get stored. If a subsequent instance is instantiated with 
 * Options::ClientSync::AT_EACH_COMMIT and tries to checkout one of the versions of the previous run, the 
 * version will be "empty" from the object-containment point of view, even though the previous run actually 
 * has objects stored whose metadata is stored everywhere. There are several alternatives to this. A simple 
 * one is to store the sync mode in the RefDB along with each version. When a version is checked out, the 
 * coordinator looks at this and can either fail or take a proactive approach and resolve the syncMode 
 * conflict.
 */

namespace versos
{
  MpiCoordinator::MpiCoordinator(RefDB& refdb, const Options& o) :
    SingleClientCoordinator(refdb, o.hash_seed), comm(o.mpi_comm), leaderRank(o.mpi_leader_rank), 
    syncMode(o.sync_mode), localRefDB("local")
  {
    init();
  }

  MpiCoordinator::MpiCoordinator(
      MPI_Comm comm,
      int leaderRank,
      Options::ClientSync::Mode syncMode,
      RefDB& refdb,
      const std::string& hashSeed) :
    SingleClientCoordinator(refdb, hashSeed), comm(comm), leaderRank(leaderRank), syncMode(syncMode), 
    localRefDB("local")
  {
    init();
  }

  void MpiCoordinator::init()
  {
    if (comm == MPI_COMM_NULL || comm == -1)
      throw std::runtime_error("MPI comm is NULL");

    if(MPI_Comm_size(comm, &size))
      throw std::runtime_error("unable to get size of MPI comm");

    if (leaderRank < 0)
      throw std::runtime_error("leader rank should be positive");

    if (leaderRank > (size - 1))
      throw std::runtime_error("leader rank shouldn't be greater than size of MPI communicator");

    if(MPI_Comm_rank(comm, &myRank))
      throw std::runtime_error("unable to get MPI rank");

    // TODO:
    //   - openDB()
    //   - initDB()

    std::string headId;

    if (imLeader())
      if (SingleClientCoordinator::getHeadId(headId))
        throw std::runtime_error("unable to get HEAD id");

    broadcast(headId);

    if (!checkout(headId).isOK())
      throw std::runtime_error("unable to synchronize HEAD");
  }

  MpiCoordinator::~MpiCoordinator()
  {
  }

  int MpiCoordinator::getHeadId(std::string& id)
  {
    if (imLeader())
      if (SingleClientCoordinator::getHeadId(id))
        handleError(-64);

    broadcast(id);

    return 0;
  }

  const Version& MpiCoordinator::checkout(const std::string& id)
  {
    const Version* v;
    std::string parentId;
    boost::ptr_set<VersionedObject> objects;

    // TODO: check if version is in localRefDB first

    if (imLeader())
    {
      v = &SingleClientCoordinator::checkout(id);

      if (!v->isOK())
        handleError(-61);

      parentId = v->getParentId();
      objects = getObjects(*(Version*)v);
    }

    broadcast(parentId);
    broadcast(objects);

    if (!imLeader())
    {
      v = new Version(id, parentId, objects);

      if (localRefDB.add(boost::shared_ptr<Version>((Version *)v)))
        handleError(-62);
    }

    return *v;
  }

  Version& MpiCoordinator::create(const Version& parent)
  {
    Version* v;

    if (imLeader())
    {
      v = &SingleClientCoordinator::create(parent);

      if (!v->isOK())
        handleError(-63);
    }
    else
    {
      v = &localRefDB.create(parent, hashSeed);
    }

    broadcast(getObjects(*v));

    return *v;
  }

  int MpiCoordinator::add(Version& v, VersionedObject& o)
  {
    if (SingleClientCoordinator::add(v, o))
      handleError(-65);

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      allGather(getObjects(v));

    return 0;
  }

  int MpiCoordinator::remove(Version& v, VersionedObject& o)
  {
    if (SingleClientCoordinator::remove(v, o))
      handleError(-66);

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      allGather(getObjects(v));

    return 0;
  }

  int MpiCoordinator::commit(Version& v)
  {
    if (syncMode == Options::ClientSync::AT_EACH_COMMIT ||
        syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      allGather(getObjects(v));

    int ret = 0;

    if (imLeader())
      ret = SingleClientCoordinator::commit(v);
    else
      ret = localRefDB.commit(v);

    if (ret)
      handleError(ret);

    waitForAll();

    return 0;
  }

  bool MpiCoordinator::isRepositoryEmpty()
  {
    int isEmpty;

    if (imLeader())
    {
      if(SingleClientCoordinator::isRepositoryEmpty())
        isEmpty = 0;
      else
        isEmpty = 1;
    }

    broadcast(&isEmpty);

    return isEmpty == 0;
  }

  int MpiCoordinator::shutdown()
  {
    if (imLeader())
      return SingleClientCoordinator::shutdown();

    return 0;
  }

  //////
  // MPI-specific
  //////

  bool MpiCoordinator::imLeader() const
  {
    return myRank == leaderRank;
  }

  void MpiCoordinator::handleError(int code) const
  {
    MPI_Abort(comm, code);
  }

  void MpiCoordinator::waitForAll() const
  {
    if(MPI_Barrier(comm))
      MPI_Abort(comm, -67);
  }

  void MpiCoordinator::broadcast(int* value) const
  {
    if (MPI_Bcast(value, 1, MPI_INT, leaderRank, comm))
      MPI_Abort(comm, -68);
  }

  void MpiCoordinator::broadcast(std::string& id) const
  {
    std::vector<char> id_vec(40);

    if (imLeader())
      id_vec = std::vector<char>(id.begin(), id.end());

    id_vec.push_back('\0');

    if (MPI_Bcast(&id_vec[0], 40, MPI_CHAR, leaderRank, comm))
      MPI_Abort(comm, -69);

    id = &id_vec[0];
  }

  void MpiCoordinator::broadcast(boost::ptr_set<VersionedObject>& objects) const
  {
    unsigned int numBytes;
    char* raw;

    if (imLeader())
    {
      // serialize objects
      std::stringstream ss;
      boost::archive::binary_oarchive oarch(ss);
      oarch << objects;

      numBytes = ss.str().size();
      raw = (char *) ss.str().c_str();
    }

    // send/receive chunk size
    if (MPI_Bcast(&numBytes, 1, MPI_UNSIGNED, leaderRank, comm))
      MPI_Abort(comm, -70);

    // send/receive the object set
    if (MPI_Bcast(&raw[0], numBytes, MPI_CHAR, leaderRank, comm))
      MPI_Abort(comm, -71);

    if (!imLeader())
    {
      // de-serialize it
      std::stringstream ss(raw);
      boost::archive::binary_iarchive iarch(ss);
      iarch >> objects;
    }
  }

  void MpiCoordinator::allGather(boost::ptr_set<VersionedObject>& objects) const
  {
    // serialize the local objects
    std::stringstream ssLocal;
    boost::archive::binary_oarchive oarch(ssLocal);
    oarch << objects;

    // get the total number of bytes across all ranks
    int localNumBytes = ssLocal.str().size();
    int globalNumBytes;

    if (MPI_Allreduce(&localNumBytes, &globalNumBytes, 1, MPI_INT, MPI_SUM, comm))
      MPI_Abort(comm, -72);

    // send/receive the whole chunk containing all sets
    char* localBytes = (char*) ssLocal.str().c_str();
    char globalBytes[globalNumBytes];

    if (MPI_Allgather(localBytes, localNumBytes, MPI_CHAR, globalBytes, globalNumBytes, MPI_CHAR, comm))
      MPI_Abort(comm, -73);

    std::stringstream ssGlobal(globalBytes);

    // de-serialize, one set at a time (one set per rank)
    for (int i = 0; i < size; ++i)
    {
      boost::ptr_set<VersionedObject> objectsForRank;
      boost::archive::binary_iarchive iarch(ssGlobal);
      iarch >> objectsForRank;

      objects.insert(objectsForRank);
    }
  }
}
