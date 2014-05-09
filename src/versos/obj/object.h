#ifndef VERSIONED_OBJECT_H
#define VERSIONED_OBJECT_H

#include "versos/versosexception.h"

#include <list>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace versos
{
/**
 * For cases when an object needs complex representation (native or stl types are not enough). For example, 
 * this could wrap a protobuff message in order to add functionality on top of it.
 */
class Object
{
protected:
  std::string oid;

public:
  Object(const std::string& oid) : oid(oid) {}
  virtual ~Object() {}

  std::string getId() const throw (VersosException) {return oid;}

  virtual const std::list<std::pair<int, int> > getModifiedOffsets() const throw (VersosException) = 0;
  virtual const std::string serialize() const throw (VersosException) = 0;

protected:
  Object();
};
}
#endif
