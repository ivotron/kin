#ifndef VERSIONED_OBJECT_H
#define VERSIONED_OBJECT_H

#include <set>
#include <sstream>
#include <string>

#include <boost/noncopyable.hpp>

namespace versos
{
  class Version;

  /**
   * Versioned object abstraction.
   *
   * note: this is not pure virtual so that it can be used in boost::ptr_container's
   */
  class VersionedObject : boost::noncopyable
  {
  protected:
    std::string interfaceName;
    std::string repositoryName;
    std::string baseName;

  public:
    VersionedObject(
        const std::string& interfaceName, const std::string& repositoryName, const std::string& baseName);

    virtual ~VersionedObject();

    /**
     * creates an object based on the given parent. From a high-level point of view, this signals the 
     * beginning of writes to the given object.
     */
    virtual int create(const Version& parent, const Version& child) = 0;

    /**
     * marks the object as committed. From a high-level point of view, this signals the end of writes to the 
     * given object. Future attempts to write to the object will fail.
     */
    virtual int commit(const Version& v) = 0;
    /**
     * removes the given version of this object.
     */
    virtual int remove(const Version& v) = 0;

    const std::string& getInterfaceName() const;
    const std::string& getBaseName() const;
    const std::string& getRepositoryName() const;

    VersionedObject* clone() const;

  protected:
    VersionedObject( const VersionedObject& );
    void operator=( const VersionedObject& );

    std::string getId(const Version& v) const;

    bool isVersionOK(const Version& v) const;

  private:
    virtual VersionedObject* do_clone() const = 0;
  };

  VersionedObject* new_clone( const VersionedObject& vo );

  bool operator==( const VersionedObject& l, const VersionedObject& r );

  bool operator<( const VersionedObject& l, const VersionedObject& r );
}
#endif
