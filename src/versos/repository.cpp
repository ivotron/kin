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

  const Version& Repository::checkout(const std::string& id)
  {
    return coordinator->checkout(id);
  }

  const Version& Repository::checkoutHEAD()
  {
    std::string headId;

    if (coordinator->getHeadId(headId) < 0)
      return Version::ERROR;

    return checkout(headId);
  }

  Version& Repository::create(const std::string& parentId)
  {
    return coordinator->create(checkout(parentId));
  }
}
