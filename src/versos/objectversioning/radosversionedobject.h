/**
 * versioned object implementation for RADOS.
 */

#ifndef RADOS_VERSIONED_OBJECT_H
#define RADOS_VERSIONED_OBJECT_H

#include "versos/objectversioning/versionedobject.h"

#include <string>
#include <rados/librados.hpp>

namespace versos
{
  class RadosVersionedObject : public VersionedObject
  {
  private:
    librados::IoCtx& ioctx;

  public:
    RadosVersionedObject(
        librados::IoCtx& ctx, const std::string& repositoryName, const std::string& baseName);
    RadosVersionedObject(
        librados::IoCtx& ctx, const std::string& repositoryName, const char* baseName);
    virtual ~RadosVersionedObject() {};

    // inherited
    int remove(const Version& v);
    int create(const Version& v);

    // rados-specific

    int write(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off);
    int read(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off);
    int stat(const Version& v, uint64_t *psize, time_t *pmtime);
    int setxattr(const Version& v, const char *name, librados::bufferlist& bl);

    // TODO: map all librados methods for objects
    //
    //   int <rados-method>(const Version& v, ...);
    //
    // with the above, the user can do
    //
    //   o.setxattr(v, ...);
    //
    // instead of doing:
    //
    //   ioctx.setxattr(o.getId(v), ...);
    //
    // just syntax sugar :)
  private:
    virtual VersionedObject* do_clone() const;
    std::string getId(const Version& v) const;
  };
}

#endif

