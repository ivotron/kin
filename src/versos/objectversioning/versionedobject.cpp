#include "versos/objectversioning/versionedobject.h"

#include "versos/version.h"
#include "versos/repository.h"

BOOST_CLASS_EXPORT_KEY(versos::VersionedObject);
BOOST_CLASS_EXPORT_IMPLEMENT(versos::VersionedObject);

namespace versos
{
  /**
   * Converts 'anything' to string.
   */
  template <typename T> static std::string to_str(T anything)
  {
    std::ostringstream ss;
    ss << anything;
    return ss.str();
  }

  VersionedObject::VersionedObject(
      const std::string& interfaceName,
      const std::string& repoName,
      const std::string& baseName,
      ContainmentVerification check) :
    interfaceName(interfaceName),
    repoName(repoName),
    baseName(baseName),
    checkForContainment(check)
  {
  }

  VersionedObject::VersionedObject(
      const std::string& interfaceName,
      const Repository& repo,
      const std::string& baseName,
      ContainmentVerification check) :
    interfaceName(interfaceName),
    repoName(repo.getName()),
    baseName(baseName),
    checkForContainment(check)
  {
  }

  VersionedObject::~VersionedObject()
  {
  }

  int VersionedObject::getId(const Version& v, std::string& id) const
  {
    if (!v.isOK())
      return -90;

    if (checkForContainment == VERIFY_CONTAINMENT && !v.contains(*this))
      // TODO: if this check becomes expensive, we can add an option to trust the user. This means that a user 
      // won't operate inconsistently in terms of intra-transaction object metadata operations, eg. remove an 
      // object and then read/write to it
      return -91;

    // TODO: maintain a cache of generated ids, so that we don't have this concatenation overhead
    id = interfaceName + "_" + repoName + "_" + baseName + "_" + to_str(v.getId());

    return 0;
  }

  const std::string& VersionedObject::getInterfaceName() const
  {
    return interfaceName;
  }
  const std::string& VersionedObject::getBaseName() const
  {
    return baseName;
  }
  const std::string& VersionedObject::getRepositoryName() const
  {
    return repoName;
  }

  VersionedObject* VersionedObject::clone() const
  {
    return do_clone();
  }

  VersionedObject* new_clone( const VersionedObject& vo )
  {
      return vo.clone();
  }

  bool operator==( const VersionedObject& l, const VersionedObject& r )
  {
    if (l.getInterfaceName() == r.getInterfaceName() &&
        l.getRepositoryName() == r.getRepositoryName() &&
        l.getBaseName() == r.getBaseName())
      return true;

    return false;
  }

  bool operator<( const VersionedObject& l, const VersionedObject& r )
  {
    if (l.getInterfaceName() == r.getInterfaceName())
    {
      if (l.getRepositoryName() == r.getRepositoryName())
        return l.getBaseName() < r.getBaseName();

      return l.getRepositoryName() < r.getRepositoryName();
    }
    else
    {
      return l.getInterfaceName() < r.getInterfaceName();
    }
  }
}
