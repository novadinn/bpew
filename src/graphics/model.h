#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

struct Model {
    bool loadFromPath(const char* path);
	
    void processNode(const std::string& directory, aiNode *node, const aiScene *scene);
    Mesh processMesh(const std::string& directory, aiMesh *mesh, const aiScene *scene);

    void destroyMeshes();
    
    std::vector<Mesh> meshes;
    // TODO: those are already stored in the mesh structures
    // std::vector<std::pair<std::string, Texture2D>> loaded_textures;
};

#endif // MODEL_H
