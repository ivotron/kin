#include "versos/coordination/mpicoordinator.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"
#include "versos/refdb/refdb.h"

#include <sstream>
#include <stdexcept>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/ptr_container/ptr_set.hpp>
#include <boost/ptr_container/serialize_ptr_set.hpp>
#include <boost/serialization/serialization.hpp>

/* high-level description of the implementation:
 *
 * Every rank has an instance of an in-memory meta DB (localRefDB) which is kept synchronized with the actual 
 * refdb at the backend. This is mainly used to manage memory allocation and mapping of version IDs to @c 
 * Version class instances.
 *
 * There is one leader rank that contacts the refdb at the backend and propagates the info to the other ranks. 
 * The info propagated is the parent-child relationship of versions as well as their IDs. Depending on the 
 * synchronization mode, the metadata about the objects contained in a version might also be synchronized. The 
 * points of synchronization are:
 *
 *   1. A version is checked out (object metadata synchronized only if syncMode != NONE)
 *   2. An object is removed/added (object metadata synchronized only if syncMode == AT_EACH_ADD_OR_REMOVE)
 *   3. A version is committed (object metadata synchronized only if syncMode == AT_EACH_COMMIT)
 *
 * TODO: currently there is no way of identifying conflicts w.r.t. distinct syncMode's used and the actual 
 * contents of a version. For example, an instance of the mpi coordinator might have operated in NONE mode and 
 * committed some versions, thus no object-containment information would have been stored. If a subsequent 
 * instance is instantiated with AT_EACH_COMMIT and tries to checkout one of the versions of the previous run, 
 * the version will be "empty" from the object-containment point of view, even though the previous run 
 * actually has objects stored whose metadata is managed externally by an app. There are several alternatives 
 * to this. A simple one is to store the sync mode in the RefDB along with each version. When a version is 
 * checked out, the coordinator looks at this, compares against the mode that it was instantiated with and can 
 * either fail or take a proactive approach in order to resolve any possible syncMode conflict.
 *
 * TODO: disallow add() and remove() for syncMode == NONE
 *
 * TODO: currently, there is a restriction that only allows to add the same number of objects at each rank per 
 * transaction (due to Allgather's requirement)
 *
 * TODO: optimize object syncing by sending only the deltas (additions and removals)
 *
 * TODO: for AT_EACH_ADD_OR_REMOVE, object metadata is synchronized among clients but not stored immediately 
 * at the backend refDB. We can add a flag to signal that when syncMode is AT_EACH_ADD_OR_REMOVE the version 
 * at the backend should be updated after every syncing.
 */

namespace versos
{
  MpiCoordinator::MpiCoordinator(RefDB& refdb, const Options& o) :
    SingleClientCoordinator(refdb, o), comm(o.mpi_comm), leaderRank(o.mpi_leader_rank), syncMode(o.sync_mode), 
    localRefDB("local")
  {
    init();
  }

  void MpiCoordinator::init()
  {
    if (comm == MPI_COMM_NULL || comm == -1)
      throw std::runtime_error("MPI comm is NULL");

    if(MPI_Comm_size(comm, &commSize))
      throw std::runtime_error("unable to get size of MPI comm");

    if (leaderRank < 0)
      throw std::runtime_error("leader rank should be positive");

    if (leaderRank > (commSize - 1))
      throw std::runtime_error("leader rank shouldn't be greater than size of MPI communicator");

    if(MPI_Comm_rank(comm, &myRank))
      throw std::runtime_error("unable to get MPI rank");

    // TODO:
    //   - openDB()
    //   - initDB()
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
    // check if locally available
    {
      const Version& localV = localRefDB.checkout(id);

      if (localV.isOK())
        return localV;
    }

    std::string parentId;
    boost::ptr_set<VersionedObject> objects;

    // ask for the version contents to leader
    {
      if (imLeader())
      {
        const Version& leaderV = SingleClientCoordinator::checkout(id);

        if (!leaderV.isOK())
          handleError(-61);

        parentId = leaderV.getParentId();
      }

      broadcast(parentId);
      broadcast(objects);
    }

    // add it to the local db
    {
      Version* v = new Version(id, parentId, objects);

      // add to local db
      int ret = localRefDB.add(boost::shared_ptr<Version>(v));

      if (ret)
        handleError(ret);

      return *v;
    }
  }

  Version& MpiCoordinator::create(const Version& parent)
  {
    if (syncMode == Options::ClientSync::NONE && parent.size() != 0)
      handleError(-62);

    if (!localRefDB.checkout(parent.getId()).isOK())
      handleError(-63);

    if (imLeader() && SingleClientCoordinator::create(parent) == Version::ERROR)
      handleError(-64);

    // since parent is here, we don't need to synchronize with the leader (in other words, singlecoordinator 
    // should create the same version id and since it has all the object metadata needed, the leader can work 
    // on its own)
    Version& newVersion = localRefDB.create(parent, hashSeed);

    return newVersion;
  }

  int MpiCoordinator::add(Version& v, VersionedObject& o)
  {
    if (syncMode == Options::ClientSync::NONE)
      return -1;

    int ret = v.add(o);
    if (ret)
      handleError(ret);

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      allGather(v);

    ret = o.create(checkout(v.getParentId()), v);
    if (ret)
      handleError(ret);

    return 0;
  }

  int MpiCoordinator::remove(Version& v, VersionedObject& o)
  {
    if (syncMode == Options::ClientSync::NONE)
      return -1;

    int ret = v.remove(o);
    if (ret)
      handleError(ret);

    if (syncMode == Options::ClientSync::AT_EACH_ADD_OR_REMOVE)
      allGather(v);

    ret = o.remove(v);
    if (ret)
      handleError(ret);

    return 0;
  }

  int MpiCoordinator::commit(Version& v)
  {
    if (syncMode == Options::ClientSync::AT_EACH_COMMIT)
      allGather(v);

    // if (syncMode == Options::ClientSync::NONE)
      // TODO: leader doesn't know about objects in each rank, so each rank has to call o.commit(v)
      //       the problem is that we don't have the references to those objects, so the user has to do that 
      //       on his/her own. We can enforce this by checking if the object being committed (locally) has 
      //       been committed in the objectstore (by adding some sort of ::VersionedObject::isCommited() 
      //       method

    if (imLeader())
    {
      int ret = SingleClientCoordinator::commit(v);

      if (ret)
        handleError(ret);
    }

    v.setStatus(Version::COMMITTED);

    barrier();

    return 0;
  }

  int MpiCoordinator::makeHEAD(const Version& v)
  {
    if (imLeader())
    {
      int ret = SingleClientCoordinator::makeHEAD(v);

      if (ret)
        handleError(ret);
    }

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

  void MpiCoordinator::barrier() const
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
    char* raw;

    if (imLeader())
      raw = (char*) id.c_str();
    else
      raw = new char[41];

    if (MPI_Bcast(raw, 41, MPI_CHAR, leaderRank, comm))
      MPI_Abort(comm, -69);

    if (!imLeader())
    {
      id = std::string(raw);
      delete raw;
    }
  }

  void MpiCoordinator::broadcast(boost::ptr_set<VersionedObject>& objects) const
  {
    if (objects.size() == 0)
      return;

    unsigned int numBytes;
    char* raw;

    if (imLeader())
    {
      // serialize objects
      std::stringstream ss;
      boost::archive::text_oarchive oarch(ss);
      oarch << objects;

      numBytes = ss.str().size();
      raw = const_cast<char*>(ss.str().data()); // OK to cast, we won't write to it
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
      boost::archive::text_iarchive iarch(ss);
      iarch >> objects;
    }
  }

  void MpiCoordinator::allGather(Version& v) const
  {
    int localNumObjects = v.getAdded().size() + v.getRemoved().size();
    int globalNumObjects;

    if (localNumObjects == 0)
      return;

    if (MPI_Allreduce(&localNumObjects, &globalNumObjects, 1, MPI_INT, MPI_SUM, comm))
    MPI_Abort(comm, -72);

    if (globalNumObjects != (commSize * localNumObjects))
      MPI_Abort(comm, -73);

    // serialize the local objects
    std::stringstream ssLocal;
    try
    {
      boost::archive::text_oarchive oarch(ssLocal);
      oarch << v.getAdded();
      oarch << v.getRemoved();
    }
    catch (...)
    {
      MPI_Abort(comm, -76);
    }

    // send the local chunk/receive the whole chunk containing all sets
    std::string strL = ssLocal.str();
    int localNumBytes = strL.size();
    char* localBytes = const_cast<char*>(strL.data()); // OK to cast, we won't write to it
    char globalBytes[localNumBytes * commSize];

    if (MPI_Allgather(localBytes, localNumBytes, MPI_CHAR, globalBytes, localNumBytes, MPI_CHAR, comm))
      MPI_Abort(comm, -74);

    std::stringstream ssGlobal(globalBytes);

    // de-serialize, one pair of sets at a time (added/removed per rank)
    for (int i = 0; i < commSize; ++i)
    {
      boost::ptr_set<VersionedObject> addedObjects;
      boost::ptr_set<VersionedObject> removedObjects;

      try
      {
        boost::archive::text_iarchive iarch(ssGlobal);
        iarch >> addedObjects;
        iarch >> removedObjects;
      }
      catch (...)
      {
        MPI_Abort(comm, -75);
      }

      boost::ptr_set<VersionedObject>::iterator it;

      for (it = addedObjects.begin(); it != addedObjects.end(); ++it)
        if (!v.contains(*it))
          v.add(*it);

      for (it = removedObjects.begin(); it != removedObjects.end(); ++it)
        if (v.contains(*it))
          v.remove(*it);
    }

    // TODO: throw an exception if a rank added/removed the same object
  }
}
