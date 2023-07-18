#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "../../core/log.h"
#include "../../core/utils.h"
#include "../../graphics/material.h"
#include "../../graphics/mesh.h"
#include "../../graphics/texture.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <string>
#include <vector>

struct MeshComponent {

  bool loadFromPath(const char *path) {
    meshes.clear();

    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      LOG_ERROR("Error while loading model at path %s: %s", path,
                importer.GetErrorString());
      return false;
    }

    std::string directory = Utils::pathDirectory(path);

    processNode(directory, scene->mRootNode, scene);
    return true;
  }

  void processNode(const std::string &directory, aiNode *node,
                   const aiScene *scene) {
    for (int i = 0; i < node->mNumMeshes; ++i) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(directory, mesh, scene));
    }
    for (int i = 0; i < node->mNumChildren; ++i) {
      processNode(directory, node->mChildren[i], scene);
    }
  }

  Mesh processMesh(const std::string &directory, aiMesh *mesh,
                   const aiScene *scene) {
    Mesh result;

    std::vector<VertexAttribute> attribs;
    if (mesh->HasPositions()) {
      attribs.push_back({sizeof(float), 3, GL_FALSE});
    }
    if (mesh->HasNormals()) {
      attribs.push_back({sizeof(float), 3, GL_FALSE});
    }
    if (mesh->HasTextureCoords(0)) {
      attribs.push_back({sizeof(float), 2, GL_FALSE});
    }
    if (mesh->HasTangentsAndBitangents()) {
      attribs.push_back({sizeof(float), 3, GL_FALSE});
      attribs.push_back({sizeof(float), 3, GL_FALSE});
    }

    result.attributes = attribs;

    for (int i = 0; i < mesh->mNumVertices; ++i) {
      if (mesh->HasPositions()) {
        result.vertices.push_back(mesh->mVertices[i].x);
        result.vertices.push_back(mesh->mVertices[i].y);
        result.vertices.push_back(mesh->mVertices[i].z);
      }

      if (mesh->HasNormals()) {
        result.vertices.push_back(mesh->mNormals[i].x);
        result.vertices.push_back(mesh->mNormals[i].y);
        result.vertices.push_back(mesh->mNormals[i].z);
      }

      // TODO: check other coordinate sets
      if (mesh->HasTextureCoords(0)) {
        result.vertices.push_back(mesh->mTextureCoords[0][i].x);
        result.vertices.push_back(mesh->mTextureCoords[0][i].y);
      }
      if (mesh->HasTangentsAndBitangents()) {
        result.vertices.push_back(mesh->mTangents[i].x);
        result.vertices.push_back(mesh->mTangents[i].y);
        result.vertices.push_back(mesh->mTangents[i].z);

        result.vertices.push_back(mesh->mBitangents[i].x);
        result.vertices.push_back(mesh->mBitangents[i].y);
        result.vertices.push_back(mesh->mBitangents[i].z);
      }

      // TODO: HasVertexColors(), HasFaces(), HasBones()
    }

    for (int i = 0; i < mesh->mNumFaces; ++i) {
      aiFace face = mesh->mFaces[i];
      for (int j = 0; j < face.mNumIndices; j++)
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
    // 		std::string full_path = Utils::connectPathWithDirectory(directory,
    // path);

    // 		bool skip = false;
    // 		for(int k = 0; k < loaded_textures.size(); ++k) {
    // 			if(strcmp(loaded_textures[k].first.data(), full_path.c_str()) == 0)
    // { 				textures.push_back(loaded_textures[k].second); 				skip = true; 				break;
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
  Material processMaterial(const std::string &directory, aiMaterial *aimat,
                           const aiScene *scene) {
    aiString name;
    aiColor3D diffuse;
    aiColor3D specular;
    aiColor3D ambient;
    aiColor3D emissive;
    aiColor3D transparent;
    int wireframe;
    int blend_mode;
    float opacity;
    float shininess_strength;
    if (AI_SUCCESS != aimat->Get(AI_MATKEY_NAME, name)) {
      printf("Failed to get material name\n");
    }
    if (AI_SUCCESS != aimat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
      printf("Failed to get material color diffuse\n");
    }
    aimat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    aimat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    aimat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
    aimat->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
    aimat->Get(AI_MATKEY_BLEND_FUNC, blend_mode);
    aimat->Get(AI_MATKEY_OPACITY, opacity);
    aimat->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);

    Material material;
    material.createDefault();
    material.name = name.C_Str();

    // material.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
    // material.specular = glm::vec3(specular[0], specular[1], specular[2]);
    // material.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
    // material.emissive = glm::vec3(emissive[0], emissive[1], emissive[2]);
    // material.transparent = glm::vec3(transparent[0], transparent[1],
    // transparent[2]); material.blend_mode = blend_mode; material.opacity =
    // opacity; material.shininess_strength = shininess_strength;

    Node *prev_diffuse_node = nullptr;
    for (int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; ++i) {
      for (int j = 0; j < aimat->GetTextureCount((aiTextureType)i); ++j) {
        aiString str;
        float strength = 1.0f;
        int operation = 0;

        if (AI_SUCCESS !=
            aimat->Get(AI_MATKEY_TEXTURE((aiTextureType)i, j), str)) {
          printf("Failed to get material texture\n");
          continue;
        }
        std::string path = str.C_Str();

        if (AI_SUCCESS !=
            aimat->Get(AI_MATKEY_TEXBLEND((aiTextureType)i, j), strength)) {
          printf("Failed to get material texblend\n");
        }
        if (AI_SUCCESS !=
            aimat->Get(AI_MATKEY_TEXOP((aiTextureType)i, j), operation)) {
          printf("Failed to get material texop\n");
        }

        std::string full_path =
            Utils::connectPathWithDirectory(directory, path);

        bool skip = false;

        Texture2D *tex = Texture2D::getTexture(full_path);

        if (!tex) {
          tex = new Texture2D();

          tex->createFromFile(full_path.c_str());
        }

        switch ((aiTextureType)i) {
        case aiTextureType_DIFFUSE: {
          if (prev_diffuse_node) {
          }
        } break;
        // case aiTextureType_SPECULAR:
        //     break;
        default:
          // TODO: handle other types of textures
          break;
        }
      }
    }

    return material;
  }

  void destroyMeshes() {
    for (int i = 0; i < meshes.size(); ++i) {
      meshes[i].destroy();
    }
  }

  // NOTE: this is temporary
  void setMaterial(uint index) {
    for (int i = 0; i < meshes.size(); ++i) {
      meshes[i].active_material_index = index;
    }
  }

  int getMaterial(int index) {
    for (int i = 0; i < materials.size(); ++i) {
      if (materials[i] == index) {
        return i;
      }
    }

    return -1;
  }

  bool hasMaterial(int index) { return getMaterial(index) != -1; }

  void addMaterial(int index) {
    if (!hasMaterial(index)) {
      materials.push_back(index);
    }
  }

  void removeMaterial(int index) {
    int i = getMaterial(index);

    if (i != -1) {
      for (auto &mesh : meshes) {
        if (mesh.active_material_index == index) {
          return;
        }
      }
      materials.erase(materials.begin() + i);
    }
  }

  std::vector<uint> materials;

  std::vector<Mesh> meshes;
};

#endif // MESH_COMPONENT_H
