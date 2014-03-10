#include "versos/refdb/redisrefdb.h"

#include "versos/version.h"
#include "versos/options.h"
#include "versos/utils.h"

#include "versos/objectversioning/versionedobject.h"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

namespace versos
{
/**
 * high-level description of implementation:
 *
 * redis keys/datatypes used:
 *   - @c <repoName>_metadb = string
 *   - @c <repoName>_head = string
 *   - @c <repoName>_<version_id>_parent = string
 *   - @c <repoName>_<version_id>_counter = string as int (a.k.a. counter)
 *   - @c <repoName>_<version_id>_objects = set
 *
 * @c <repoName>_metadb determines whether metadata for a repo exists (i.e. whether is empty), if the 
 * associated string is empty the repo is empty then the repo is too. The @c <repoName>_head contains the id 
 * of the HEAD. @c <repoName>_<version_id>_counter is used to handle the shared locks and implicitly the 
 * transaction status (0 means the version is committed). @c <repoName>_<version_id>_objects is a set that 
 * contains object ids (generated through Version::getId).
 *
 * Operations are almost 1-to-1 with redis::DB. When a version is created (via RefDB::create which in turn 
 * invokes RedisRefDB::own), the counter is increased by one. If a version is committed, the counter is 
 * decreased. This operations are atomic so we don't need to worry about race conditions among participants. 
 * The only thing to note is that the ::makeHead() isn't currently atomic (eg. should use Redis' MULTI or Lua 
 * scripts). When an object is added to a version, the object is added to the @c repoName_versionId_objects 
 * set; similarly for removals.
 *
 * TODO: RefDB::EXCLUSIVE_LOCK is not supported (might be trivial to implement but we haven't gone through how 
 * it would get done)
 * TODO: @c lockKeys are currently ignored
 */
RedisRefDB::RedisRefDB(const std::string& repoName, const Options& o) :
  MemRefDB(repoName), host(o.metadb_server_address)
{
}

RedisRefDB::~RedisRefDB()
{
}

int RedisRefDB::open()
{
  try
  {
    redisdb.connect(host);
  }
  catch (...)
  {
    return -100;
  }

  return 0;
}

int RedisRefDB::close()
{
  redisdb.disconnect();

  return 0;
}

bool RedisRefDB::isEmpty() const
{
  std::string value = redisdb.get(MemRefDB::repoName + "_metadb");

  return value.empty();
}

int RedisRefDB::makeHEAD(const Version& v)
{
  // TODO: make this atomic
  // {
  if (v.getParentId() != getHeadId())
    return -54;
  redisdb.set(repoName + "_head", v.getId());
  // }

  redisdb.set(repoName + "_" + v.getId() + "_parent", v.getParentId());

  return 0;
}

int RedisRefDB::commit(const Version& v)
{
  return redisdb.decr(repoName + "_" + v.getId() + "_counter");
}

int RedisRefDB::getLockCount(const Version& v, const std::string&)
{
  // TODO: check lock key
  return getLockCount(v.getId());
}

int RedisRefDB::getLockCount(const std::string& id)
{
  std::string getted = redisdb.get(repoName + "_" + id + "_counter");
  std::cout << "getted: " << getted << std::endl << std::flush;
  return boost::lexical_cast<int>(getted);
}

const Version& RedisRefDB::checkout(const std::string& id)
{
  const Version& v = MemRefDB::checkout(id);

  if (v.isOK())
    return v;

  if (getLockCount(id) != 0)
    // not committed
    return Version::ERROR;

  std::string parentId = redisdb.get(repoName + "_" + id + "_parent");
  std::list<std::string> objIds = redisdb.lrange(repoName + "_" + id + "_objects 0 -1");
  boost::ptr_set<VersionedObject> objects;

  for(std::list<std::string>::iterator it = objIds.begin(); it != objIds.end(); ++it)
  {
    std::vector<std::string> objectMeta = Utils::split(*it, '_');

    if (objectMeta.size() != 3)
      throw std::runtime_error("Expecting 3 elements but got " + objectMeta.size());

    objects.insert(new VersionedObject(objectMeta[0], objectMeta[1], objectMeta[2]));
  }

  Version* checkedOutVersion = new Version(id, parentId, objects);

  MemRefDB::add(boost::shared_ptr<Version>(checkedOutVersion));

  return *checkedOutVersion;
}

int RedisRefDB::add(const Version& v, const VersionedObject& o)
{
  std::string oid;

  if (o.getId(v, oid))
    return -103;

  if (redisdb.sadd(repoName + "_" + v.getId() + "_objects", oid) != 1)
    return -104;

  return 0;
}

int RedisRefDB::remove(const Version& v, const VersionedObject& o)
{
  std::string oid;

  if (o.getId(v, oid))
    return -103;

  if (redisdb.srem(repoName + "_" + v.getId() + "_objects", oid) != 1)
    return -104;

  return 0;
}

int RedisRefDB::add(const Version& v, const boost::ptr_set<VersionedObject>& o)
{
  std::list<std::string> ids;

  boost::ptr_set<VersionedObject>::iterator it;

  for (it = o.begin(); it != o.end(); ++it)
  {
    std::string oid;

    if (it->getId(v, oid))
      return -103;

    ids.push_back(oid);
  }

  if (redisdb.sadd(repoName + "_" + v.getId() + "_objects", ids) != (long long) ids.size())
    return -105;

  return 0;
}

int RedisRefDB::remove(const Version& v, const boost::ptr_set<VersionedObject>& o)
{
  std::list<std::string> ids;

  boost::ptr_set<VersionedObject>::iterator it;

  for (it = o.begin(); it != o.end(); ++it)
  {
    std::string oid;

    if (it->getId(v, oid))
      return -103;

    ids.push_back(oid);
  }

  if (redisdb.srem(repoName + "_" + v.getId() + "_objects", ids) != (long long) ids.size())
    return -105;

  return 0;
}

int RedisRefDB::remove(const Version& )
{
  return -108;
}

int RedisRefDB::own(boost::shared_ptr<Version> v, LockType lock, const std::string&)
{
  if (lock == RefDB::EXCLUSIVE_LOCK)
    return -109;

  redisdb.incr(repoName + "_" + v->getId() + "_counter");

  MemRefDB::add(v);

  return 0;
}

} // namespace
