/**
 * in-memory implementation of a versioned object.
 */

#ifndef MEM_VERSIONED_OBJECT_H
#define MEM_VERSIONED_OBJECT_H

#include "versos/objectversioning/versionedobject.h"

#include <map>
#include <string>

namespace versos
{
  class Repository;

  class MemVersionedObject : public VersionedObject
  {
  private:
    std::map<std::string, std::string> values;
  public:
    MemVersionedObject();
    MemVersionedObject(const std::string& repoName, const std::string& baseName);
    MemVersionedObject(const Repository& repo, const std::string& baseName);

    virtual ~MemVersionedObject();

    // inherited
    int create(const Version& parent, const Version& child);
    int commit(const Version& v);
    int remove(const Version& v);

    // mem-specific
    int write(const Version& v, const std::string& value);
    int read(const Version& v, std::string& value);

  private:
    virtual VersionedObject* do_clone() const;
  };
}
#endif
