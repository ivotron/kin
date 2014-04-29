#ifndef VERSIONED_OBJECT_H
#define VERSIONED_OBJECT_H

#include "versos/versosexception.h"

#include <set>
#include <sstream>
#include <string>

// NOTE: this has to include the actual archives that are to be used ANYWHERE in the library, otherwise a 
// runtime exception is thrown by boost. So if a coordinator uses a binary archive, it has to be included 
// here. If another one uses text archives, it has to be included here, and so on. We include text by default 
// wince those are platform independent
// {
#ifdef ENABLE_MPI_COORDINATOR
  #include <boost/mpi/packed_oarchive.hpp>
  #include <boost/mpi/packed_iarchive.hpp>
#endif
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
// }
#include <boost/noncopyable.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>

namespace versos
{
  class Version;
  class Repository;

  /**
   * Versioned object abstraction. An implementation should be able to work properly by having only the three 
   * members of this class (@c repositoryName and @c baseName) since these are the only pieces of information 
   * stored in the reference database (i.e. the object metadata). Users always instantiate 
   * implementation-specific objects prior to operating on them, that is, they can never get a reference to an 
   * object that is contained in a @c Version. This is a design decision that allows to avoid having to deal 
   * with implementation-specific issues as part of the @c versos library.
   *
   * Since users can interact directly with derived-specific methods, the implementation of these should check 
   * for a version's state (status and special instances) in order to avoid issues. For example, a @c 
   * FooObject class implementing a method @c write(Version v, std::string value) should check that the given 
   * version is valid, e.g. via the @c Version::isOK() method. The provided @c getId(Version) method actually 
   * makes this @c Version::isOK() check before generating the ID of the object.
   */
  class Object : boost::noncopyable
  {
    friend class boost::serialization::access;

  protected:
    std::string oid;

  public:
    Object(const std::string& oid);
    virtual ~Object();

    std::string getId() const throw (VersosException);

    Object* clone() const;

  protected:
    Object();
    Object( const Object& );
    void operator=( const Object& );

  private:
    virtual Object* do_clone() const = 0;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int)
    {
      ar & oid;
    }
  };

  Object* new_clone( const Object& vo );

  bool operator==( const Object& l, const Object& r );

  bool operator<( const Object& l, const Object& r );
}
#endif
