#include "versos/refdb/refdb.h"

#include "versos/version.h"
#include "versos/objectversioning/versionedobject.h"

#include <sstream>
#include <iomanip>
#include <stdexcept>

#include <openssl/sha.h>

namespace versos
{
  RefDB::RefDB(const std::string& repoName) : repoName(repoName)
  {
    if (repoName.empty())
      throw std::runtime_error("RefDB: empty repo name");
  }

  RefDB::~RefDB()
  {
  }

  int RefDB::init()
  {
    Version v(Version::PARENT_FOR_ROOT);

    if (insert(v) < 0)
      return -80;

    if (commit(v) < 0)
      return -81;

    if (makeHEAD(v) < 0)
      return -82;

    headId = v.getId();

    return 0;
  }

  const std::string& RefDB::getHeadId() const
  {
    return headId;
  };

  const Version& RefDB::checkout(const std::string& id)
  {
    Version& v = get(id);

    if (!v.isCommitted())
      return Version::ERROR;

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

    if (insert(v, lock, lockKey) < 0)
      return Version::ERROR;

    return get(v.getId());
  }

  int RefDB::insert(Version& v)
  {
    return insert(v, EXCLUSIVE_LOCK, "");
  }
}
