#include "versos/coordination/coordinator.h"

#include "versos/version.h"

namespace versos
{
  boost::ptr_set<VersionedObject>& Coordinator::getObjects(Version& v)
  {
    return v.getObjects();
  }
}