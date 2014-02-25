#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>

//#include <boost/serialization/serialization.hpp>
//#include <boost/serialization/map.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>

class Utils
{
public:
  /**
   * Converts 'anything' to string.
   */
  template <typename T> static std::string to_str(T anything)
  {
    std::ostringstream ss;
    ss << anything;
    return ss.str();
  }

  /*
  static bool file_exists(const std::string& filename)
  {
    std::ifstream f(filename.c_str());
    return f.good();
  }

  template <typename K, typename V> static void to_file(
      const std::map<K,V>& map, const std::string& filename)
  {
    std::ofstream f(filename.c_str());
    boost::archive::binary_oarchive oarch(f);
    oarch << map;
  }

  template <typename K, typename V> static void from_file(
      std::map<K,V>& map, const std::string& filename)
  {
    std::ifstream f(filename.c_str());
    boost::archive::binary_iarchive iarch(f);
    iarch >> map;
  }
   */
};

#endif
