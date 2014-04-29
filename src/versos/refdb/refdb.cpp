#include "versos/refdb/refdb.h"

#include "versos/version.h"

#include <sstream>
#include <iomanip>
#include <stdexcept>

#include <openssl/sha.h>

namespace versos
{
  RefDB::RefDB(const std::string& repoName) : repoName(repoName)
  {
    if (repoName.empty())
      throw VersosException("RefDB: empty repo name");
  }

  RefDB::~RefDB()
  {
  }

  void RefDB::init() throw (VersosException)
  {
    Version v(Version::PARENT_FOR_ROOT);

    // we create in shared lock to avoid race conditions for repositories that are initialized by a parallel 
    // application (i.e. a version with multiple participants). For example: an MPI app using the 
    // BackendCoordinator can experience race conditions when initializing the repo. By having the repo 
    // created in shared mode, we just wait for the last to commit.
    insert(v, SHARED_LOCK, "");
    commit(v);
    makeHEAD(v);

    headId = v.getId();
  }

  const std::string& RefDB::getHeadId() const
  {
    return headId;
  };

  const Version& RefDB::checkout(const std::string& id) throw (VersosException)
  {
    Version& v = get(id);

    if (!v.isOK())
      return v;

    if (!v.isCommitted())
      throw VersosException("Version " + id + " not committed");

    return v;
  }

  Version& RefDB::create(const Version& parent, const std::string& hashSeed)
  {
    return create(parent, hashSeed, EXCLUSIVE_LOCK, "");
  }

  Version& RefDB::create(
      const Version& parent, const std::string& hashSeed, LockType lock, const std::string& lockKey)
  {
    // get SHA1 for new child
    unsigned char childSHA1[20];

    std::string d = parent.getId() + hashSeed;

    SHA1((const unsigned char*) d.c_str(), d.size(), childSHA1);

    std::string childHash(childSHA1, childSHA1 + 20);

    // get the HEX representation
    std::stringstream ss;
    for(int i = 0; i < 20; i++)
      ss << std::hex << std::setw(2) << std::setfill('0') << (int)childSHA1[i];

    // instantiate
    Version v(ss.str(), parent);

    insert(v, lock, lockKey);

    return get(v.getId());
  }

  void RefDB::insert(Version& v) throw (VersosException)
  {
    insert(v, EXCLUSIVE_LOCK, "");
  }
}
