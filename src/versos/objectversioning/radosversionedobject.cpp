#include "versos/objectversioning/radosversionedobject.h"

#include <sstream>

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

  RadosVersionedObject::RadosVersionedObject(
        librados::IoCtx& ctx, const std::string& repositoryName, const std::string& baseName) :
    VersionedObject("rados", repositoryName, baseName), ioctx(ctx)
  {
  }

  RadosVersionedObject::RadosVersionedObject(
        librados::IoCtx& ctx, const std::string& repositoryName, const char* baseName) :
    VersionedObject("rados", repositoryName, std::string(baseName)), ioctx(ctx)
  {
  }

  std::string RadosVersionedObject::getId(const Version& v) const
  {
    if (v == Version::NOT_FOUND || v == Version::ERROR || v == Version::EMPTY)
      return "";

    if (!v.contains(*this))
      // TODO: if this check becomes expensive, we can add an option to trust the user. This means that a user 
      // won't write to an object in a version and then remove it
      return "";

    // TODO: maintain a cache of generated ids, so that we don't have this concatenation overhead
    return interfaceName + "_" + repositoryName + "_" + baseName + "_" + to_str(v.getId());
  }

  int RadosVersionedObject::write(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off)
  {
    if (v.getStatus() == Version::COMMITTED)
      return -1;

    std::string oid = getId(v);

    if (oid.empty())
      return -2;

    return ioctx.write(oid, bl, len, off);
  }

  int RadosVersionedObject::read(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off)
  {
    std::string oid = getId(v);

    if (oid.empty())
      return -2;

    return ioctx.read(oid, bl, len, off);
  }

  int RadosVersionedObject::stat(const Version& v, uint64_t *psize, time_t *pmtime)
  {
    std::string oid = getId(v);

    if (oid.empty())
      return -2;

    return ioctx.stat(oid, psize, pmtime);
  }

  int RadosVersionedObject::setxattr(const Version& v, const char *name, librados::bufferlist& bl)
  {
    std::string oid = getId(v);

    if (oid.empty())
      return -2;

    return ioctx.setxattr(oid, name, bl);
  }

  int RadosVersionedObject::remove(const Version& v)
  {
    std::string oid = getId(v);

    if (oid.empty())
      return -2;

    return ioctx.remove(oid);
  }

  int RadosVersionedObject::create(const Version&)
  {
    return 0;
  }

  VersionedObject* RadosVersionedObject::do_clone() const
  {
    return new RadosVersionedObject(ioctx, repositoryName, baseName);
  }
}
