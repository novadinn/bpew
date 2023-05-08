#include "model.h"

#include "../core/log.h"

bool Model::loadModel(const char* path) {
	Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
					     aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("Error while loading model at path %s: %s", path, importer.GetErrorString());
		return false;
    }

    processNode(scene->mRootNode, scene);
	return true;
}

void Model::processNode(aiNode *node, const aiScene *scene) {
	for(int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
    }
    for(int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	Mesh result;

	std::vector<VertexAttribute> attribs;
	attribs.push_back({sizeof(float), 3, GL_FALSE});
	if(mesh->HasNormals()) {
		attribs.push_back({sizeof(float), 3, GL_FALSE});
	}
	if(mesh->mTextureCoords[0]) {
		attribs.push_back({sizeof(float), 2, GL_FALSE});
		attribs.push_back({sizeof(float), 3, GL_FALSE});
		attribs.push_back({sizeof(float), 3, GL_FALSE});
	}

	result.attributes = attribs;
	
    for(int i = 0; i < mesh->mNumVertices; ++i) {
		result.vertices.push_back(mesh->mVertices[i].x);
		result.vertices.push_back(mesh->mVertices[i].y);
		result.vertices.push_back(mesh->mVertices[i].z);
	    
		if (mesh->HasNormals()) {
			result.vertices.push_back(mesh->mNormals[i].x);
			result.vertices.push_back(mesh->mNormals[i].y);
			result.vertices.push_back(mesh->mNormals[i].z);
		}

		if(mesh->mTextureCoords[0]) {
			result.vertices.push_back(mesh->mTextureCoords[0][i].x);
			result.vertices.push_back(mesh->mTextureCoords[0][i].y);

			result.vertices.push_back(mesh->mTangents[i].x);
			result.vertices.push_back(mesh->mTangents[i].y);
			result.vertices.push_back(mesh->mTangents[i].z);

			result.vertices.push_back(mesh->mBitangents[i].x);
			result.vertices.push_back(mesh->mBitangents[i].y);
			result.vertices.push_back(mesh->mBitangents[i].z);
		}
    }

    for(int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for(int j = 0; j < face.mNumIndices; j++)
			result.indices.push_back(face.mIndices[j]);
    }

	result.generateVertexArray();
	
    return result;
}
