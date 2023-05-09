#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "platform.h"

#include <string>

struct FileSystem {
	static std::string joinPath(const char* path);
	static std::string pathDirectory(const char* path);
	static std::string connectPathWithDirectory(const std::string& path, const std::string& directory);
};

#endif
