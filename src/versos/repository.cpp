#include "versos/repository.h"

namespace versos
{
  Repository::Repository(const std::string& name, Coordinator& coordinator) :
    name(name), coordinator(coordinator.clone())
  {
  }

  const Version& Repository::checkout(const std::string& id) const
  {
    return coordinator->checkout(id);
  }

  int Repository::init()
  {
    return coordinator->init();
  }

  const Version& Repository::checkoutHEAD() const
  {
    std::string headId;

    if (coordinator->getHeadId(headId))
      return Version::ERROR;

    return checkout(headId);
  }

  Version& Repository::create(const std::string& parentId)
  {
    return coordinator->create(checkout(parentId));
  }

  void Repository::close()
  {
  }
}
