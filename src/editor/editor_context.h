#ifndef EDITOR_CONTEXT_H
#define EDITOR_CONTEXT_H

#include "../ecs/components/tag_component.h"
#include "../ecs/entity.h"
#include "../ecs/scene.h"
#include "../graphics/framebuffer.h"
#include "../graphics/renderer_context.h"
#include "editor_camera.h"

#include "glm/glm.hpp"

#include <map>

struct SpaceLayoutData;
struct SpaceModelingData;
struct SpaceShadingData;

struct SceneNode {
  SceneNode *parent = nullptr;
  bool collection = false;
  Entity entity;
  std::vector<SceneNode> children;
};

struct EditorContext {
  RendererContext *renderer_context;
  Scene *scene;
  EditorCamera *editor_camera;
  Entity active_entity;
  std::vector<Entity> selected_entities;
  std::pair<Entity, int> active_vertex;
  std::vector<std::pair<Entity, int>> selected_vertices;
  SceneNode scene_tree;
  SceneNode *active_collection = nullptr;

  SpaceLayoutData *space_layout_data;
  SpaceModelingData *space_modeling_data;
  SpaceShadingData *space_shading_data;

  void createNode() {
    SceneNode node;
    node.entity = scene->createEntity();
    node.parent = active_collection;

    ASSERT(active_collection);
    addChild(active_collection, node);
    selected_entities.clear();
    selected_entities.push_back(node.entity);
    active_entity = node.entity;
  }

  void moveNode(Entity entity, SceneNode *to) {
    SceneNode *node_p = findNodeByEntity(entity, scene_tree);

    if (node_p) {
      SceneNode *child = findNodeByEntity(to->entity, *node_p);

      if (child) {
        return;
      }
      SceneNode node = *node_p;
      SceneNode *from_node = node.parent;

      for (int i = 0; i < from_node->children.size(); ++i) {
        if (from_node->children[i].entity == node.entity) {
          from_node->children.erase(from_node->children.begin() + i);
          break;
        }
      }

      replaceParentReq(*from_node, from_node->parent);

      to = findNodeByEntity(to->entity, scene_tree);

      addChild(to, node);
    }
  }

  void addChild(SceneNode *node, SceneNode child) {
    node->children.push_back(child);

    replaceParentReq(*node, node->parent);
  }

  void replaceParentReq(SceneNode &node, SceneNode *parent) {
    node.parent = parent;
    for (int i = 0; i < node.children.size(); ++i) {
      replaceParentReq(node.children[i], &node);
    }
  }

  SceneNode *collectionLookup(SceneNode *node) {
    if (node->collection) {
      return node;
    }

    return collectionLookup(node->parent);
  }

  void createCollection() {
    SceneNode node;
    node.collection = true;
    node.entity = scene->createEntity("Collection");
    node.parent = active_collection;

    ASSERT(active_collection);
    addChild(active_collection, node);
    selected_entities.clear();
    selected_entities.push_back(node.entity);
    active_entity = node.entity;
  }

  void selectNode(SceneNode *node) {
    active_collection = collectionLookup(node);
    selected_entities.push_back(node->entity);
    active_entity = node->entity;
  }

  SceneNode *findNodeByEntity(Entity entity, SceneNode &node) {
    if (node.entity == entity) {
      return &node;
    }
    for (int i = 0; i < node.children.size(); ++i) {
      SceneNode *finded_node = findNodeByEntity(entity, node.children[i]);
      if (finded_node) {
        return finded_node;
      }
    }
    return nullptr;
  }

  void moveNodeOut(Entity entity) {
    SceneNode *node = findNodeByEntity(entity, scene_tree);
    if (node->parent) {

      SceneNode *collection = collectionLookup(node->parent);

      moveNode(entity, collection);
    }
  }

  void destroyNode() {
    if (!active_entity) {
      return;
    }
    SceneNode *active_node_p = findNodeByEntity(active_entity, scene_tree);

    if (active_node_p) {
      if (active_node_p->parent) {
        SceneNode active_node = *active_node_p;
        if (active_node.collection) {
          active_collection = collectionLookup(active_node.parent);
        }

        for (int i = 0; i < selected_entities.size(); ++i) {
          if (selected_entities[i] == active_entity) {
            selected_entities.erase(selected_entities.begin() + i);
            break;
          }
        }

        for (int i = 0; i < active_node.children.size(); ++i) {
          active_node.parent->children.push_back(active_node.children[i]);
        }

        for (int i = 0; i < active_node.parent->children.size(); ++i) {
          if (active_node.parent->children[i].entity == active_entity) {
            active_node.parent->children.erase(
                active_node.parent->children.begin() + i);
            break;
          }
        }

        replaceParentReq(*active_node.parent, active_node.parent->parent);

        scene->destroyEntity(active_entity);
        active_entity = {};
      }
    }
  }

  bool entitySelected(Entity entity) {
    bool contains = false;
    for (int i = 0; i < selected_entities.size(); ++i) {
      if (selected_entities[i] == entity) {
        contains = true;
        break;
      }
    }

    return contains;
  }

  bool vertexSelected(std::pair<Entity, int> vertex) {
    bool contains = false;
    for (int i = 0; i < selected_vertices.size(); ++i) {
      if (selected_vertices[i] == vertex) {
        contains = true;
        break;
      }
    }

    return contains;
  }
};

#endif // EDITOR_CONTEXT_H
