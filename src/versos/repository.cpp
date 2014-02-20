#include "versos/repository.h"

namespace versos
{
  Repository::Repository(const std::string& name, Coordinator& coordinator) :
    name(name), coordinator(coordinator.clone())
  {
  }

  Repository::~Repository()
  {
  }

  const std::string& Repository::getName() const
  {
    return name;
  }

  const Version& Repository::checkout(const std::string& id) const
  {
    return coordinator->checkout(id);
  }

  bool Repository::isEmpty() const
  {
    return coordinator->isRepositoryEmpty();
  }

  int Repository::init()
  {
    return coordinator->initRepository();
  }

  const Version& Repository::checkoutHEAD() const
  {
    std::string headId;

    if (coordinator->getHeadId(headId))
      return Version::ERROR;

    return checkout(headId);
  }

  Version& Repository::create(const Version& parent)
  {
    return coordinator->create(parent);
  }

  Version& Repository::create(const std::string& parentId)
  {
    return coordinator->create(checkout(parentId));
  }

  void Repository::close()
  {
  }
}
