#include "versos/obj/object.h"

#include "versos/version.h"
#include "versos/repository.h"

BOOST_CLASS_EXPORT_KEY(versos::Object);
BOOST_CLASS_EXPORT_IMPLEMENT(versos::Object);

namespace versos
{
  /**
   * Converts 'anything' to string.
   */
  template <typename T> static std::string to_str(T anything)
  {
    std::ostringstream ss;
    ss << anything;
    return ss.str();
  }

  Object::Object()
  {
  }

  Object::Object(const std::string& oid) : oid(oid)
  {
  }

  Object::~Object()
  {
  }

  std::string Object::getId() const throw (VersosException)
  {
    return oid;
  }

  Object* Object::clone() const
  {
    return do_clone();
  }

  Object* new_clone( const Object& vo )
  {
      return vo.clone();
  }

  bool operator==( const Object& l, const Object& r )
  {
    if (l.getId() == r.getId())
      return true;

    return false;
  }

  bool operator<( const Object& l, const Object& r )
  {
    return l.getId() < r.getId();
  }
}
