#include "versos/refdb/refdb.h"

namespace versos
{
  RefDB RefDB::NONE = RefDB();

  RefDB::RefDB()
  {
    noneOrValid = "valid";
  }

  RefDB::RefDB(std::string noneOrValid) : noneOrValid(noneOrValid)
  {
  }

  RefDB::~RefDB() {}
  int RefDB::init() {return 0;}
  bool RefDB::isEmpty() const {return true;}
  const std::string& RefDB::getHeadId() const {return noneOrValid;}
  const Version& RefDB::checkout(const std::string&) {return Version::ERROR;}
  Version& RefDB::create(const Version&, Coordinator&, const std::string&) {return Version::ERROR;}

  int RefDB::remove(const Version&) {return 0;}
  /**
   * lock a revision. modes = EX or SHARED
   */
  int RefDB::lock(const Version&, int ) {return 0;}
  int RefDB::commit(const Version&) {return 0;}

  RefDB* RefDB::clone() {return new RefDB(noneOrValid);}
}
