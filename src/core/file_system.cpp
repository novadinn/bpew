#include "file_system.h"

#include <cstring>

std::string FileSystem::joinPath(const char* path) {
	
#if defined(PLATFORM_WINDOWS)
	const char slash_char = '\\';
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_IOS) || defined(PLATFORM_MACOS) || defined(PLATFORM_ANDROID)
	const char slash_char = '/';
#endif
	
	std::string result;
		
	for (int i = 0; i < strlen(path); ++i) {
		if (path[i] == '/' || path[i] == '\\') {
			result += slash_char;
			continue;
		}

		result += path[i];
	}

	return result;
}
