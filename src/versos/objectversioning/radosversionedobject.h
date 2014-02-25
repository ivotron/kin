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
    RadosVersionedObject(librados::IoCtx& ctx, const std::string& repoName, const std::string& baseName);
    RadosVersionedObject(librados::IoCtx& ctx, const Repository& repo, const std::string& baseName);
    RadosVersionedObject(librados::IoCtx& ctx, const Repository& repo, const char* baseName);
    virtual ~RadosVersionedObject();

    // inherited
    int create(const Version& parent, const Version& child);
    int commit(const Version& v);
    int remove(const Version& v);

    // rados-specific
    int write(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off);
    int read(const Version& v, librados::bufferlist& bl, size_t len, uint64_t off);
    int stat(const Version& v, uint64_t *psize, time_t *pmtime);
    int setxattr(const Version& v, const char *name, librados::bufferlist& bl);
  private:
    virtual VersionedObject* do_clone() const;
  };
}
#endif
