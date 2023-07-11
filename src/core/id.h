#ifndef ID_H
#define ID_H

#include "utils.h"

#include <string>

struct ID {
  void create(std::string id_name) {
    name = id_name;
    // TODO: cannot do this right now, since imgui and imnodes uses ints as ids
    // id = Utils::generateUUID();
    static int global_id = 0;
    id = global_id++;
  }

  std::string name;
  int id;
};

#endif // ID_H
