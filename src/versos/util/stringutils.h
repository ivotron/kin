#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class StringUtils
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
};

#endif
