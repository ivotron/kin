#ifndef VERSIONED_OBJECT_H
#define VERSIONED_OBJECT_H

#include "versos/version.h"

#include <string>
#include <set>

#include <boost/noncopyable.hpp>

namespace versos
{
  /**
   * Versioned object abstraction.
   */
  class VersionedObject : boost::noncopyable
  {
  protected:
    std::string interfaceName;
    std::string repositoryName;
    std::string baseName;

  public:
    VersionedObject(
        const std::string& interfaceName, const std::string& repositoryName, const std::string& baseName) :
      interfaceName(interfaceName), repositoryName(repositoryName), baseName(baseName){}

    virtual ~VersionedObject() {}

    /**
     * removes the specified version of this object.
     */
    virtual int remove(const Version& v) = 0;

    virtual int create(const Version& v) = 0;

    const std::string& getInterfaceName() const { return interfaceName; }
    const std::string& getBaseName() const { return baseName; }
    const std::string& getRepositoryName() const { return repositoryName; }

    VersionedObject* clone() const { return do_clone(); }

  protected:
    VersionedObject( const VersionedObject& ) { }
    void operator=( const VersionedObject& );

  private:
    virtual VersionedObject* do_clone() const = 0;

    // TODO: add methods common to all objects . For example:
    //
    //   virtual int write(const Version& v, ...);
    //   virtual int read(const Version& v, ...);
    //
    // the above allows the user to do:
    //
    //   o.write(v, ...);
    //
    // instead of
    //
    //   ioctx.write(o.getId(v), ...);
    //
    // just syntactic sugar :)
  };

  VersionedObject* new_clone( const VersionedObject& vo )
  {
      return vo.clone();
  }

  inline bool operator==( const VersionedObject& l, const VersionedObject& r )
  {
    if (l.getInterfaceName() == r.getInterfaceName() &&
        l.getRepositoryName() == r.getRepositoryName() &&
        l.getBaseName() == r.getBaseName())
      return true;

    return false;
  }

  inline bool operator<( const VersionedObject& l, const VersionedObject& r )
  {
    if (l.getInterfaceName() == r.getInterfaceName())
    {
      if (l.getRepositoryName() == r.getRepositoryName())
        return l.getBaseName() < r.getBaseName();

      return l.getRepositoryName() < r.getRepositoryName();
    }
    else
    {
      return l.getInterfaceName() < r.getInterfaceName();
    }
  }

  /*
   * TODO: add RBD images (which IMO should be called something else, eg. 
   * stripped blocks, sharded objects, etc)

   class RbdVersionedObject : public VersionedObject
   {
   public:
   RbdVersionedObject(const std::string& id);
   const std::string& getImage();
   };
   */
}
#endif
