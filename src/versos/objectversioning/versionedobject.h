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
   * members of this class (@c interfaceName, @c repositoryName and @c baseName) since these are the only 
   * pieces of information stored in the reference database (i.e. the object metadata). Users always 
   * instantiate implementation-specific objects prior to operating on them, that is, they can never get a 
   * reference to an object that is contained in a @c Version. This is a design decision that allows to avoid 
   * having to deal with implementation-specific issues as part of the @c versos library. This also means that 
   * serializing an object effectively slices it, i.e. all the implementation-specific information is lost. 
   * The assumption here being that the only reason why an object is being serialized is for metadata-handling 
   * purposes and thus only the three base class' members should be enough to instantiate the object somewhere 
   * else.
   *
   * Since users can interact directly with derived-specific methods, the implementation of these should check 
   * for a version's state (status and special instances) in order to avoid issues. For example, a @c 
   * FooVersionedObject class implementing a method @c write(Version v, std::string value) should check that 
   * the given version is valid, e.g. via the @c Version::isOK() method. The provided @c getId(Version) method 
   * actually makes this @c Version::isOK() check before generating the ID of the object.
   */
  class VersionedObject : boost::noncopyable
  {
    friend class boost::serialization::access;

  protected:
    std::string interfaceName;
    std::string repoName;
    std::string baseName;
    int id;

  public:
    VersionedObject(const std::string& interfaceName, const Repository& repo, const std::string& baseName);
    VersionedObject(const std::string& interfaceName, const std::string& repoName, const std::string& base);
    virtual ~VersionedObject();

    /**
     * creates an object based on the given parent. From a high-level point of view, this signals the 
     * beginning of writes to the given object.
     *
     * We can safely assume that the versions are safe to be operated on (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Coordinator should have done this check already.
     */
    virtual void create(const Version& parent, const Version& child) throw (VersosException);

    /**
     * marks the object as committed. From a high-level point of view, this signals the end of writes to the 
     * given object. Future attempts to write to the object will fail.
     *
     * We can safely assume that the version is safe to commit (i.e. it's not a Version::NOT_FOUND, etc..), 
     * since @c Repository has already done this check.
     */
    virtual void commit(const Version& v) throw (VersosException);

    /**
     * removes the given version of this object.
     *
     * We can safely assume that the version is safe to be removed (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Repository has already done this check.
     */
    virtual void remove(const Version& v) throw (VersosException);

    const std::string& getInterfaceName() const;
    const std::string& getBaseName() const;
    const std::string& getRepositoryName() const;
    std::string getId(const Version& v) const throw (VersosException);

    VersionedObject* clone() const;

  protected:
    VersionedObject();
    VersionedObject( const VersionedObject& );
    void operator=( const VersionedObject& );

  private:
    virtual VersionedObject* do_clone() const;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int)
    {
      ar & interfaceName;
      ar & repoName;
      ar & baseName;
    }
  };

  VersionedObject* new_clone( const VersionedObject& vo );

  bool operator==( const VersionedObject& l, const VersionedObject& r );

  bool operator<( const VersionedObject& l, const VersionedObject& r );
}
#endif
