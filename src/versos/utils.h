#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

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

  static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
  {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
  }

  static std::vector<std::string> split(const std::string &s, char delim)
  {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
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
