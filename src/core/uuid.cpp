#include "uuid.h"

#include <random>
#include <unordered_map>

static std::random_device random_device;
static std::mt19937_64 engine(random_device());
std::uniform_int_distribution<uint64> uniform_distribution;

void UUID::create() { uuid = uniform_distribution(engine); }

void UUID::createFromUUID(uint64 uuid_) { uuid = uuid_; }