#include "model.h"

#include "../core/log.h"
#include "../core/file_system.h"

#include <iostream>

bool Model::loadFromPath(const char* path) {
	Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
					     aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("Error while loading model at path %s: %s", path, importer.GetErrorString());
		return false;
    }

	std::string directory = FileSystem::pathDirectory(path);
	
    processNode(directory, scene->mRootNode, scene);
	return true;
}

void Model::processNode(const std::string& directory, aiNode *node, const aiScene *scene) {
	for(int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(directory, mesh, scene));
    }
    for(int i = 0; i < node->mNumChildren; ++i) {
		processNode(directory, node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(const std::string& directory, aiMesh *mesh, const aiScene *scene) {
	Mesh result;

	std::vector<VertexAttribute> attribs;
	if(mesh->HasPositions()) {
		attribs.push_back({sizeof(float), 3, GL_FALSE});
	}
	if(mesh->HasNormals()) {
		attribs.push_back({sizeof(float), 3, GL_FALSE});
	}
	if(mesh->HasTextureCoords(0)) {
		attribs.push_back({sizeof(float), 2, GL_FALSE});
	}
	if(mesh->HasTangentsAndBitangents()) {
		attribs.push_back({sizeof(float), 3, GL_FALSE});
		attribs.push_back({sizeof(float), 3, GL_FALSE});
	}

	result.attributes = attribs;
	
    for(int i = 0; i < mesh->mNumVertices; ++i) {
		if(mesh->HasPositions()) {
			result.vertices.push_back(mesh->mVertices[i].x);
			result.vertices.push_back(mesh->mVertices[i].y);
			result.vertices.push_back(mesh->mVertices[i].z);
		}
	    
		if(mesh->HasNormals()) {
			result.vertices.push_back(mesh->mNormals[i].x);
			result.vertices.push_back(mesh->mNormals[i].y);
			result.vertices.push_back(mesh->mNormals[i].z);
		}
		
		// TODO: check other coordinate sets
		if(mesh->HasTextureCoords(0)) {
			result.vertices.push_back(mesh->mTextureCoords[0][i].x);
			result.vertices.push_back(mesh->mTextureCoords[0][i].y);
		}
		if(mesh->HasTangentsAndBitangents()) {
			result.vertices.push_back(mesh->mTangents[i].x);
			result.vertices.push_back(mesh->mTangents[i].y);
			result.vertices.push_back(mesh->mTangents[i].z);

			result.vertices.push_back(mesh->mBitangents[i].x);
			result.vertices.push_back(mesh->mBitangents[i].y);
			result.vertices.push_back(mesh->mBitangents[i].z);
		}
		
		// TODO: HasVertexColors(), HasFaces(), HasBones()
    }

    for(int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for(int j = 0; j < face.mNumIndices; j++)
			result.indices.push_back(face.mIndices[j]);
    }

	result.generateVertexArray();

	// aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; 
	// std::vector<Texture2D> textures;

	// for(int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; ++i) {
	// 	for(int j = 0; j < material->GetTextureCount((aiTextureType)i); ++j) {
	// 		aiString str;
	// 		material->GetTexture((aiTextureType)i, j, &str);

	// 		std::string path = str.C_Str();
	// 		std::string full_path = FileSystem::connectPathWithDirectory(directory, path);
			
	// 		bool skip = false;
	// 		for(int k = 0; k < loaded_textures.size(); ++k) {
	// 			if(strcmp(loaded_textures[k].first.data(), full_path.c_str()) == 0) {
	// 				textures.push_back(loaded_textures[k].second);
	// 				skip = true;
	// 				break;
	// 			}
	// 		}

	// 		if(!skip) {	
	// 			Texture2D texture;
	// 			texture.createFromFile(full_path.c_str());

	// 			textures.push_back(texture);
	// 			loaded_textures.push_back({full_path, texture});
	// 		}
	// 	}
	// }

    return result;
}
