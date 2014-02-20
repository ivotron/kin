#include "versos/objectversioning/radosversionedobject.h"

#include "versos/version.h"

#include <sstream>

namespace versos
{

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
  RadosVersionedObject::~RadosVersionedObject()
  {
  }

  int RadosVersionedObject::create(const Version& p, const Version& c)
  {
    std::string oid;

    oid = getId(p);

    if (oid.empty())
      return -1;

    oid = getId(c);

    if (oid.empty())
      return -2;

    // TODO: we currently do nothing else since we assume that the contents of the file are entirely 
    // overwritten from one version to the other.
    //
    // TODO: in order to efficiently address the above, we have to use use rados_clone_range()

    return 0;
  }

  int RadosVersionedObject::commit(const Version&)
  {
    // TODO: if create() uses rados_clone_range() and snapshots the whole object, we don't need to do anything
    //
    // TODO: if write() is using rados_clone_range() instead, we have to fill the holes
    return 0;
  }

  int RadosVersionedObject::remove(const Version& v)
  {
    std::string oid = getId(v);

    if (oid.empty())
      return -2;

    return ioctx.remove(oid);
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

  VersionedObject* RadosVersionedObject::do_clone() const
  {
    return new RadosVersionedObject(ioctx, repositoryName, baseName);
  }
}
