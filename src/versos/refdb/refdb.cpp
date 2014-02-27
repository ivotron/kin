#include "versos/refdb/refdb.h"

#include "versos/version.h"

#include <sstream>
#include <iomanip>

#include <openssl/sha.h>

namespace versos
{
  RefDB::RefDB(const std::string& repoName) : repoName(repoName)
  {
  }

  RefDB::~RefDB()
  {
  }

  int RefDB::init()
  {
    boost::shared_ptr<Version> v(new Version(Version::PARENT_FOR_ROOT));

    if (own(v))
      return -80;

    headId = v->getId();

    return 0;
  }

  const std::string& RefDB::getHeadId() const
  {
    return headId;
  };

  Version& RefDB::create(const Version& parent, const std::string& hashSeed)
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
    boost::shared_ptr<Version> v(new Version(ss.str(), parent));

    if (own(v))
      return Version::ERROR;

    return *v;
  }
}
