#include "versos/objectversioning/radosversionedobject.h"

#include "versos/version.h"

#include <sstream>

BOOST_CLASS_EXPORT_KEY(versos::RadosVersionedObject);
BOOST_CLASS_EXPORT_IMPLEMENT(versos::RadosVersionedObject);

namespace versos
{
  librados::IoCtx RadosVersionedObject::forDefaultConstructor;

  RadosVersionedObject::RadosVersionedObject() :
    VersionedObject("rados", repoName, baseName), ioctx(forDefaultConstructor)
  {
  }

  RadosVersionedObject::RadosVersionedObject(
        librados::IoCtx& ctx, const std::string& repoName, const std::string& baseName) :
    VersionedObject("rados", repoName, baseName), ioctx(ctx)
  {
    boost::serialization::void_cast_register<RadosVersionedObject, VersionedObject>();
  }

  RadosVersionedObject::RadosVersionedObject(
        librados::IoCtx& ctx, const Repository& repo, const std::string& baseName) :
    VersionedObject("rados", repo, baseName), ioctx(ctx)
  {
    boost::serialization::void_cast_register<RadosVersionedObject, VersionedObject>();
  }

  RadosVersionedObject::~RadosVersionedObject()
  {
  }

  int RadosVersionedObject::create(const Version&, const Version&)
  {
    // TODO: we currently do nothing since we assume that the contents of the file are entirely overwritten 
    // from one version to the other.
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
    if (v.isCommitted())
      return -31;

    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    ret = ioctx.remove(id);

    if (ret)
      return -32;

    return 0;
  }

  int RadosVersionedObject::write(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off)
  {
    if (v.isCommitted())
      return -33;

    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    ret = ioctx.write(id, bl, len, off);

    if (ret < 0)
      return -34;

    return ret;
  }

  int RadosVersionedObject::read(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off)
  {
    if (v.isCommitted())
      return -35;

    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    ret = ioctx.read(id, bl, len, off);

    if (ret < 0)
      return -36;

    return ret;
  }

  int RadosVersionedObject::stat(const Version& v, uint64_t *psize, time_t *pmtime)
  {
    if (v.isCommitted())
      return -37;

    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    ret = ioctx.stat(id, psize, pmtime);

    if (ret)
      return -38;

    return 0;
  }

  int RadosVersionedObject::setxattr(const Version& v, const char *name, librados::bufferlist& bl)
  {
    if (v.isCommitted())
      return -39;

    std::string id;

    int ret = getId(v, id);

    if (ret)
      return ret;

    ret = ioctx.setxattr(id, name, bl);

    if (ret < 0)
      return -30;

    return 0;
  }

  void RadosObjDB::load(const Version& v, const std::string& id, Object* o) throw (VersosException)
  {
  }

  VersionedObject* RadosVersionedObject::do_clone() const
  {
    return new RadosVersionedObject(ioctx, repoName, baseName);
  }
}
