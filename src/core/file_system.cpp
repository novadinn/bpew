#include "file_system.h"

#include <cstring>

#if defined(PLATFORM_WINDOWS)
#define SLASH_CHAR '\\'
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_IOS) || defined(PLATFORM_MACOS) || defined(PLATFORM_ANDROID)
#define SLASH_CHAR '/'
#endif

std::string FileSystem::joinPath(const char* path) {
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

std::string FileSystem::pathDirectory(const char* path) {
	std::string str = path;
	return str.substr(0, str.find_last_of(SLASH_CHAR));
}

std::string FileSystem::connectPathWithDirectory(const std::string& path, const std::string& directory) {
	return path + SLASH_CHAR + directory;
}
