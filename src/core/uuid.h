#ifndef UUID_H
#define UUID_H

#include "platform.h"

#include <iostream>

struct UUID {
  void create();
  void createFromUUID(uint64 uuid_);

  operator uint64() const { return uuid; }

private:
  uint64 uuid;
};

namespace std {
template <typename T> struct hash;

template <> struct hash<UUID> {
  std::size_t operator()(const UUID &uuid) const { return (uint64)uuid; }
};
}; // namespace std

#endif // UUID_H