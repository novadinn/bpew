#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Model {
	bool loadModel(const char* path);
	
	void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	
	std::vector<Mesh> meshes;
};

#endif // MODEL_H
