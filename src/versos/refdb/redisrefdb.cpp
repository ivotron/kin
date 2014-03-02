#include "versos/refdb/redisrefdb.h"

#include "versos/version.h"
#include "versos/options.h"

#include <boost/serialization/shared_ptr.hpp>

namespace versos
{
/**
 * high-level description of implementation:
 *
 * redis keys/datatypes used:
 *   - @c <repoName>_metadb = string
 *   - @c <repoName>_head = string
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

  // TODO: "exec SET repoName_head = v.getId()"

  // }

  return 0;
}

int RedisRefDB::commit(const Version&)
{
  // return "exec DECR v.getId()_counter"

  return 0;
}

const Version& RedisRefDB::checkout(const std::string& id)
{
  const Version& v = MemRefDB::checkout(id);

  if (v.isOK())
    return v;

  // TODO: read and instantiate the version

  return Version::NOT_FOUND;
}

int RedisRefDB::add(const Version&, const VersionedObject&)
{
  // TODO: add to set, fail if 0 is returned
  return 0;
}

int RedisRefDB::remove(const Version&, const VersionedObject&)
{
  // TODO: remove from set, fail if 0 is returned
  return 0;
}

int RedisRefDB::addAll(const Version&)
{
  // TODO: add and return number of added objects
  return 0;
}

int RedisRefDB::remove(const Version& )
{
  return -108;
}

int RedisRefDB::own(boost::shared_ptr<Version> v, LockType lock, const std::string& lockKey)
{
  if (lock == RefDB::EXCLUSIVE_LOCK)
    return -109;

  // TODO: Increase create repoName_versionId_counter key

  MemRefDB::own(v, lock, lockKey);

  return 0;
}

} // namespace
