#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <map>
#include <fstream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

class Utils
{
public:
  /**
   */
  static bool file_exists(const std::string& filename)
  {
    std::ifstream f(filename.c_str());
    return f.good();
  }

  /**
   * to file
   */
  template <typename K, typename V> static void to_file(
      const std::map<K,V>& map, const std::string& filename)
  {
    std::ofstream f(filename.c_str());
    boost::archive::binary_oarchive oarch(f);
    oarch << map;
  }

  /**
   * from file
   */
  template <typename K, typename V> static void from_file(
      std::map<K,V>& map, const std::string& filename)
  {
    std::ifstream f(filename.c_str());
    boost::archive::binary_iarchive iarch(f);
    iarch >> map;
  }
};

#endif
