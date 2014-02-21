#include "versos/objectversioning/versionedobject.h"

#include "versos/version.h"
#include "versos/repository.h"

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
      const std::string& interfaceName, const Repository& repo, const std::string& baseName) :
    interfaceName(interfaceName), repo(repo), baseName(baseName)
  {
  }

  VersionedObject::~VersionedObject()
  {
  }

  std::string VersionedObject::getId(const Version& v) const
  {
    if (!isVersionOK(v))
      return "";

    if (!v.contains(*this))
      // TODO: if this check becomes expensive, we can add an option to trust the user. This means that a user 
      // won't operate inconsistently in terms of intra-transaction object metadata operations, eg. remove an 
      // object and then read/write to it
      return "";

    // TODO: maintain a cache of generated ids, so that we don't have this concatenation overhead
    return interfaceName + "_" + repo.getName() + "_" + baseName + "_" + to_str(v.getId());
  }

  bool VersionedObject::isVersionOK(const Version& v) const
  {
    return (v != Version::ERROR && v != Version::NOT_FOUND);
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
    return repo.getName();
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
