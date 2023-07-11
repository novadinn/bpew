#ifndef SHA_H
#define SHA_H

#include <iomanip>
#include <openssl/sha.h>
#include <sstream>
#include <string>

struct Sha {
  void create(const char *src, uint size) {
    SHA256_CTX sha256;
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256((const unsigned char *)src, size, hash);

    std::stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      ss << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(hash[i]);
    }

    sha = ss.str();
  }

  std::string sha = "";
};

#endif
