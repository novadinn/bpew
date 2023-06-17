#ifndef UUID_H
#define UUID_H

#include "platform.h"

#include <random>
#include <unordered_map>
#include <string>
#include <cstring>

#if defined(PLATFORM_WINDOWS)
#define SLASH_CHAR '\\'
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_IOS) || defined(PLATFORM_MACOS) || defined(PLATFORM_ANDROID)
#define SLASH_CHAR '/'
#endif

namespace Utils {
  static std::random_device random_device;
  static std::mt19937_64 mtprng(random_device());;
  static std::uniform_int_distribution<uint64_t> uniform_distribution;

  static uint64 generateUUID() {
    return uniform_distribution(mtprng);
  }

  // TODO: move those to the platform.h
  static std::string joinPath(const char* path) {
    std::string result;
		
    for (int i = 0; i < strlen(path); ++i) {
      if (path[i] == '/' || path[i] == '\\') {
	result += SLASH_CHAR;
	continue;
      }

      result += path[i];
    }

    return result;
  }

  static std::string pathDirectory(const char* path) {
    std::string str = path;
    return str.substr(0, str.find_last_of(SLASH_CHAR));
  }

  static std::string connectPathWithDirectory(const std::string& path, const std::string& directory) {
    return path + SLASH_CHAR + directory;
  }

};

#endif // UUID_H
