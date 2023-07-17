#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include "material.h"

struct MaterialManager {

    static Material* getMaterial(uint index);    
    static Material* getMaterialOrDefault(uint index);
    static void addMaterial(std::string name);
    static bool validMaterialIndex(uint index);
    static void removeMaterial(uint index);
    
    static Material default_material;
    static std::vector<Material> materials;
};

#endif
