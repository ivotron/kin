#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <map>

class Utils
{
public:

  /**
   * Converts 'anything' to string.
   */
  template <typename T> static std::string to_str(T Number)
  {
    std::ostringstream ss;
    ss << Number;
    return ss.str();
  }

  /**
   * DJB hashing function.
   *
   * TODO: substitute by SHA-1 or MD5 to minimize collisions
   */
  static int hash(const std::string& str)
  {
    unsigned int hashcode = 0;
    int offset = 'a' - 1;

    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
      hashcode = hashcode << 1 | (*it - offset);

    return hashcode;
  }
  static int hash(const char* str)
  {
    return hash(std::string(str));
  }

  /**
   * hashes a string.
   */
  static int hash_and_add(const char* str, std::map<int, std::string>& map)
  {
    std::string std_str(str);

    int oid = Utils::hash(std_str);

    map[oid] = std_str;

    return oid;
  }
};

#endif
