#include "versos/refdb/redisrefdb.h"

#include "versos/version.h"
#include "versos/options.h"
#include "versos/utils.h"

#include <boost/lexical_cast.hpp>

namespace versos
{
/**
 * high-level description of implementation:
 *
 * redis keys/datatypes used:
 *   - @c <repoName>_head = string
 *   - @c <repoName>_<version_id>_parent = string
 *   - @c <repoName>_<version_id>_locks = string as int (a.k.a. counter)
 *   - @c <repoName>_<version_id>_objects = set
 *
 * @c <repoName>_head determines whether metadata for a repo exists (i.e. whether is empty), if the associated 
 * string is empty then the repo is too. At the same time, the @c <repoName>_head contains the id of the HEAD. 
 * @c <repoName>_<version_id>_locks is used to handle the shared locks and implicitly the transaction status 
 * (0 means the version is committed). @c <repoName>_<version_id>_objects is a set that contains object ids 
 * (generated through Version::getId).
 *
 * Operations are almost 1-to-1 with redis::DB. When a version is created (via RefDB::create which in turn 
 * invokes RedisRefDB::own), the counter is increased by one. If a version is committed, the counter is 
 * decreased. This operations are atomic so we don't need to worry about race conditions among participants. 
 * The only thing to note is that the ::makeHead() method isn't currently atomic (eg. should use Redis' MULTI 
 * or Lua scripts). When an object is added to a version, the object is added to the @c 
 * repoName_versionId_objects set; similarly for removals.
 *
 * TODO: @c lockKeys are currently ignored
 */
RedisRefDB::RedisRefDB(const std::string& repoName, const Options& o) :
  MemRefDB(repoName), host(o.metadb_server_address)
{
  if (host.empty() || host == "")
    host = "127.0.0.1";
}

RedisRefDB::~RedisRefDB()
{
}

void RedisRefDB::open() throw (VersosException)
{
  try
  {
    redisdb.connect(host);
    headId = redisdb.get(repoName + "_head");
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }
}

void RedisRefDB::close() throw (VersosException)
{
  redisdb.disconnect();
}

bool RedisRefDB::isEmpty() const throw (VersosException)
{
  try
  {
    std::string value = redisdb.get(repoName + "_head");
    return value.empty();
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }
}

void RedisRefDB::makeHEAD(const Version& v) throw (VersosException)
{
  // TODO: make this atomic
  // {
  if (v.getParentId() != getHeadId())
    throw VersosException("current HEAD is not equal to version's parent");

  try
  {
    redisdb.set(repoName + "_head", v.getId());
  // }

    redisdb.set(repoName + "_" + v.getId() + "_parent", v.getParentId());
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  headId = v.getId();
}

int RedisRefDB::commit(const Version& v) throw (VersosException)
{
  MemRefDB::commit(v);

  try
  {
    return redisdb.decr(repoName + "_" + v.getId() + "_locks");
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }
}

int RedisRefDB::getLockCount(const Version& v, const std::string&) throw (VersosException)
{
  // TODO: check lock key
  return getLockCount(v.getId());
}

int RedisRefDB::getLockCount(const std::string& id) throw (VersosException)
{
  std::string cnt;

  try
  {
    cnt = redisdb.get(repoName + "_" + id + "_locks");
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  if (cnt.empty())
    throw VersosException("no _locks entry in RedisDB");

  try
  {
    return boost::lexical_cast<int>(cnt);
  }
  catch (boost::bad_lexical_cast& e)
  {
    throw VersosException(e.what());
  }
}

Version& RedisRefDB::get(const std::string& id) throw (VersosException)
{
  Version& v = MemRefDB::get(id);

  if (v != Version::NOT_FOUND)
    return v;

  if (getLockCount(id) != 0)
    throw VersosException("version not committed yet");

  std::string parentId;
  std::list<std::string> objIds;

  try
  {
    parentId = redisdb.get(repoName + "_" + id + "_parent");
    objIds = redisdb.lrange(repoName + "_" + id + "_objects 0 -1");
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  std::set<std::string> objects;

  for(std::list<std::string>::iterator it = objIds.begin(); it != objIds.end(); ++it)
  {
    std::vector<std::string> objectMeta = Utils::split(*it, '_');

    if (objectMeta.size() != 3)
      throw VersosException("Expecting 3 elements but got " + objectMeta.size());

    objects.insert(new std::string(objectMeta[0], objectMeta[1], objectMeta[2]));
  }

  Version checkedOutVersion(id, parentId, objects);

  MemRefDB::add(checkedOutVersion);

  return MemRefDB::get(id);
}

void RedisRefDB::add(const Version& v, const std::string& o) throw (VersosException)
{
  std::string oid = o.getId(v);

  int cnt;

  try
  {
    cnt = redisdb.sadd(repoName + "_" + v.getId() + "_objects", oid);
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  if (cnt != 1)
    throw VersosException("Object already in given version");
}

void RedisRefDB::remove(const Version& v, const std::string& o) throw (VersosException)
{
  std::string oid = o.getId(v);

  int cnt;

  try
  {
    cnt = redisdb.srem(repoName + "_" + v.getId() + "_objects", oid);
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  if (cnt != 1)
    throw VersosException("Object not removed from version");
}

void RedisRefDB::add(const Version& v, const std::set<std::string>& o) throw (VersosException)
{
  std::list<std::string> ids;

  std::set<std::string>::iterator it;

  for (it = o.begin(); it != o.end(); ++it)
    ids.push_back(it->getId(v));

  unsigned long cnt;

  try
  {
    cnt = redisdb.sadd(repoName + "_" + v.getId() + "_objects", ids);
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  if (cnt != ids.size())
    throw VersosException("One or more objects not added to version");
}

void RedisRefDB::remove(const Version& v, const std::set<std::string>& o) throw (VersosException)
{
  std::list<std::string> ids;

  std::set<std::string>::iterator it;

  for (it = o.begin(); it != o.end(); ++it)
    ids.push_back(it->getId(v));

  unsigned long cnt;

  try
  {
    cnt = redisdb.srem(repoName + "_" + v.getId() + "_objects", ids);
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  if (cnt != ids.size())
    throw VersosException("One or more objects not removed from version");
}

void RedisRefDB::insert(Version& v, LockType lock, const std::string& lockKey) throw (VersosException)
{
  int cnt;

  MemRefDB::insert(v, lock, lockKey);

  try
  {
    cnt = redisdb.incr(repoName + "_" + v.getId() + "_locks");
  }
  catch (std::runtime_error& e)
  {
    throw VersosException(e.what());
  }

  if (cnt == 2 && lock == RefDB::EXCLUSIVE_LOCK)
  {
    try
    {
      redisdb.decr(repoName + "_" + v.getId() + "_locks");
    }
    catch (std::runtime_error& e)
    {
      throw VersosException(e.what());
    }

    throw VersosException("Can't lock more than once in EXCLUSIVE_LOCK");
  }
}

} // namespace
