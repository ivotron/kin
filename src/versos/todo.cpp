

  static std::string SHA1(const std::string& input);

  // TODO: generate SHA1s using libcrypto++
  /*
  static std::string SHA1(const std::string source)
  {
    std::string digest;
    CryptoPP::SHA1 hash;

    CryptoPP::StringSource foo(aString, true,
        new CryptoPP::HashFilter(hash,
          new CryptoPP::Base64Encoder (
            new CryptoPP::StringSink(digest))));

    return digest;
  }
  */
