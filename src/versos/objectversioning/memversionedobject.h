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
    void create(const Version& parent, const Version& child) throw (VersosException);
    void commit(const Version& v) throw (VersosException);
    void remove(const Version& v) throw (VersosException);

    // mem-specific
    void put(const Version& v, const std::string& value) throw (VersosException);
    std::string get(const Version& v) throw (VersosException);

  private:
    virtual VersionedObject* do_clone() const;
  };
}
#endif
