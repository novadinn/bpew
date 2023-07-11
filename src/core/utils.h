#ifndef UUID_H
#define UUID_H

#include "platform.h"

#include <cstring>
#include <random>
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "imgui/imgui.h"

#if defined(PLATFORM_WINDOWS)
#define SLASH_CHAR '\\'
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_IOS) ||                      \
    defined(PLATFORM_MACOS) || defined(PLATFORM_ANDROID)
#define SLASH_CHAR '/'
#endif

namespace Utils {
static std::random_device random_device;
static std::mt19937_64 mtprng(random_device());
;
static std::uniform_int_distribution<uint64_t> uniform_distribution;

static uint64 generateUUID() { return uniform_distribution(mtprng); }

/* convert path to the machine independent format */
static std::string joinPath(const char *path) {
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

static std::string pathDirectory(const char *path) {
  std::string str = path;
  return str.substr(0, str.find_last_of(SLASH_CHAR));
}

static std::string connectPathWithDirectory(const std::string &path,
                                            const std::string &directory) {
  return path + SLASH_CHAR + directory;
}

static glm::vec2 getAvailableViewportSize() {
  ImVec2 vpsize = ImGui::GetContentRegionAvail();

  return {vpsize.x, vpsize.y};
}

static glm::vec2 getAvailableViewportBoundsMin() {
  auto viewport_min_region = ImGui::GetWindowContentRegionMin();
  auto viewport_max_region = ImGui::GetWindowContentRegionMax();
  auto viewport_offset = ImGui::GetWindowPos();

  return {viewport_min_region.x + viewport_offset.x,
          viewport_min_region.y + viewport_offset.y};
}

static glm::vec2 getAvailableViewportBoundsMax() {
  auto viewport_min_region = ImGui::GetWindowContentRegionMin();
  auto viewport_max_region = ImGui::GetWindowContentRegionMax();
  auto viewport_offset = ImGui::GetWindowPos();

  return {viewport_max_region.x + viewport_offset.x,
          viewport_max_region.y + viewport_offset.y};
}

static bool isViewportHovered() { return ImGui::IsWindowHovered(); }
}; // namespace Utils

#endif // UUID_H
