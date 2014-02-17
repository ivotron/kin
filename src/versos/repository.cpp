/**
 *
 */
#include "versos/repository.h"

namespace versos
{
  Repository::Repository(const std::string& name, Coordinator& coordinator) :
    name(name), coordinator(coordinator.clone())
  {
  }

  Version& Repository::checkout(uint64_t id)
  {
    std::map<uint64_t, Version>::iterator f;

    f = checkedOutVersions.find(id);

    if (f != checkedOutVersions.end())
      return f->second;

    Version& v = coordinator->checkout(id);

    if (v == Version::NOT_FOUND)
      return v;

    checkedOutVersions[v.getId()] = v;

    return checkedOutVersions[v.getId()];
  }

  Version& Repository::checkoutHEAD()
  {
    uint64_t headId;

    if (coordinator->getHeadId(headId) < 0)
      return Version::ERROR;

    return checkout(headId);
  }

  Version& Repository::create(uint64_t parentId)
  {
    return coordinator->create(checkout(parentId));
  }

  Version& Repository::get(uint64_t id)
  {
    return checkout(id);
  }
}
