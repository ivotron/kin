#ifndef VERSIONED_OBJECT_H
#define VERSIONED_OBJECT_H

#include <set>
#include <sstream>
#include <string>

#include <boost/noncopyable.hpp>

namespace versos
{
  class Version;
  class Repository;

  /**
   * Versioned object abstraction.
   *
   * Since users interact directly with derived-specific methods, the implementation of these should check for 
   * a version's state (status and special instances) in order to avoid issues. For example, a @c 
   * FooVersionedObject class implementing a method @c write(Version v, std::string value) should check that 
   * the given version is valid (e.g. via the @c Version::isOK() method). The provided @c getId(Version) 
   * actually makes this @c Version::isOK() check.
   */
  class VersionedObject : boost::noncopyable
  {
    // note: this is not pure virtual so that it can be used in boost::ptr_container's
  protected:
    std::string interfaceName;
    const Repository& repo;
    std::string baseName;

  public:
    VersionedObject(const std::string& interfaceName, const Repository& repo, const std::string& baseName);

    virtual ~VersionedObject();

    /**
     * creates an object based on the given parent. From a high-level point of view, this signals the 
     * beginning of writes to the given object.
     *
     * We can safely assume that the versions are safe to be operated on (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Coordinator should have done this check already.
     */
    virtual int create(const Version& parent, const Version& child) = 0;

    /**
     * marks the object as committed. From a high-level point of view, this signals the end of writes to the 
     * given object. Future attempts to write to the object will fail.
     *
     * We can safely assume that the version is safe to commit (i.e. it's not a Version::NOT_FOUND, etc..), 
     * since @c Version has already done this check.
     */
    virtual int commit(const Version& v) = 0;

    /**
     * removes the given version of this object.
     *
     * We can safely assume that the version is safe to be removed (i.e. it's not a Version::NOT_FOUND, 
     * etc..), since @c Version has already done this check.
     */
    virtual int remove(const Version& v) = 0;

    const std::string& getInterfaceName() const;
    const std::string& getBaseName() const;
    const std::string& getRepositoryName() const;

    VersionedObject* clone() const;

  protected:
    VersionedObject( const VersionedObject& );
    void operator=( const VersionedObject& );

    int getId(const Version& v, std::string& id) const;

  private:
    virtual VersionedObject* do_clone() const = 0;
  };

  VersionedObject* new_clone( const VersionedObject& vo );

  bool operator==( const VersionedObject& l, const VersionedObject& r );

  bool operator<( const VersionedObject& l, const VersionedObject& r );
}
#endif
