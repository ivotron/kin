#include "versos/objectversioning/radosversionedobject.h"

#include "versos/version.h"

#include <sstream>

namespace versos
{

  RadosVersionedObject::RadosVersionedObject(
        librados::IoCtx& ctx, const Repository& repo, const std::string& baseName) :
    VersionedObject("rados", repo, baseName), ioctx(ctx)
  {
  }

  RadosVersionedObject::RadosVersionedObject(
        librados::IoCtx& ctx, const Repository& repo, const char* baseName) :
    VersionedObject("rados", repo, std::string(baseName)), ioctx(ctx)
  {
  }
  RadosVersionedObject::~RadosVersionedObject()
  {
  }

  int RadosVersionedObject::create(const Version& p, const Version& c)
  {
    if (!isVersionOK(p) || !isVersionOK(c))
      return -1;

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
    return ioctx.remove(getId(v));
  }

  int RadosVersionedObject::write(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off)
  {
    if (v.getStatus() == Version::COMMITTED)
      return -1;

    return ioctx.write(getId(v), bl, len, off);
  }

  int RadosVersionedObject::read(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off)
  {
    return ioctx.read(getId(v), bl, len, off);
  }

  int RadosVersionedObject::stat(const Version& v, uint64_t *psize, time_t *pmtime)
  {
    return ioctx.stat(getId(v), psize, pmtime);
  }

  int RadosVersionedObject::setxattr(const Version& v, const char *name, librados::bufferlist& bl)
  {
    return ioctx.setxattr(getId(v), name, bl);
  }

  VersionedObject* RadosVersionedObject::do_clone() const
  {
    return new RadosVersionedObject(ioctx, repo, baseName);
  }
}
