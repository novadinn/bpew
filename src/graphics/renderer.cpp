#include "renderer.h"

#include "../graphics/shaders/shader.h"
#include "../core/glm_extensions.h"
#include "../core/log.h"
#include "shaders/shader_builder.h"
#include "shaders/infos/solid_shader_info.h"
#include "shaders/infos/fxaa_shader_info.h"
#include "shaders/infos/outline_selected_mesh_shader_info.h"
#include "shaders/infos/mesh_vertices_info.h"
#include "shaders/infos/outline_selected_vertex_shader_info.h"

#include <glad/glad.h>

static Shader solid_shader;
static Shader outline_selected_mesh_shader;
static Shader mesh_vertices_shader;
static Shader outline_selected_vertex_shader;
static Shader fxaa_shader;

static Mesh quad_mesh; 

void Renderer::init() {
    ShaderBuilder::buildShaderFromCreateInfo(solid_shader, solid_shader_create_info);  
    ShaderBuilder::buildShaderFromCreateInfo(fxaa_shader, fxaa_shader_create_info);
    ShaderBuilder::buildShaderFromCreateInfo(outline_selected_mesh_shader,
					     outline_selected_mesh_shader_create_info);
    ShaderBuilder::buildShaderFromCreateInfo(mesh_vertices_shader,
					     mesh_vertices_shader_create_info);
    ShaderBuilder::buildShaderFromCreateInfo(outline_selected_vertex_shader,
					     outline_selected_vertex_shader_create_info);
    
    std::vector<float> quad_vertices = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
    };

    quad_mesh.attributes = {{sizeof(float), 2, GL_FALSE}, {sizeof(float), 2, GL_FALSE}};
    quad_mesh.vertices = quad_vertices;
    quad_mesh.generateVertexArray();    
}

void Renderer::destroy() {
    solid_shader.destroy();
    fxaa_shader.destroy();
    mesh_vertices_shader.destroy();
    outline_selected_mesh_shader.destroy();
    outline_selected_vertex_shader.destroy();
    
    quad_mesh.destroy();
}

void Renderer::drawMeshMaterial(RendererContext *context) {
    Material* material = context->mesh->getActiveMaterial();
    if(!material || !material->shader_container || !material->shader_container->compiled) {
	material = &context->mesh->default_material;
   }

    // TODO: each mesh has its own material
    for(int i = 0; i < context->mesh->meshes.size(); ++i) {
	Mesh& target = context->mesh->meshes[i];
	
	material->shader_container->shader.bind();
	material->shader_container->shader.setMatrix4("model", context->model);
	material->shader_container->shader.setMatrix4("view", context->view);
	material->shader_container->shader.setMatrix4("projection", context->projection);

	material->shader_container->shader.setFloat("shininess", 0.2f);
	material->shader_container->shader.setVec3("viewPos", context->view_pos);
	material->shader_container->shader.setVec3("dirLight.direction", context->direction);	
	material->shader_container->shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	material->shader_container->shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	material->shader_container->shader.setVec3("dirLight.specilar", glm::vec3(0.5f, 0.5f, 0.5f));
	material->shader_container->shader.setInt("currentEntityID", (int)context->current_entity_id);

	for(auto& node : material->nodes) {
	    setMaterialNodeUniforms(material->shader_container->shader, node);
	}

	target.va.bind();
	glDrawElements(GL_TRIANGLES, target.indices.size(), GL_UNSIGNED_INT, 0);
	target.va.unbind();
	
	material->shader_container->shader.unbind();
    }
}

void Renderer::drawMeshSolid(RendererContext *context) {
    for(int i = 0; i < context->mesh->meshes.size(); ++i) {
	solid_shader.bind();
	solid_shader.setMatrix4("model", context->model);
	solid_shader.setMatrix4("view", context->view);
	solid_shader.setMatrix4("projection", context->projection);

	solid_shader.setFloat("shininess", 0.2f);
	solid_shader.setVec3("viewPos", context->view_pos);
	solid_shader.setVec3("dirLight.direction", context->direction);    
	solid_shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	solid_shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	solid_shader.setVec3("dirLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));
	solid_shader.setInt("currentEntityID", (int)context->current_entity_id);
		
	Mesh& target = context->mesh->meshes[i];
		
	target.va.bind();
	glDrawElements(GL_TRIANGLES, target.indices.size(), GL_UNSIGNED_INT, 0);
	target.va.unbind();

	solid_shader.unbind();
    }
}

void Renderer::drawMeshRendered(RendererContext *context) {
    Material* material = context->mesh->getActiveMaterial();
    if(!material || !material->shader_container || !material->shader_container->compiled) {
	material = &context->mesh->default_material;
    }    

    for(int i = 0; i < context->mesh->meshes.size(); ++i) {
	material->shader_container->shader.bind();
	material->shader_container->shader.setMatrix4("model", context->model);
	material->shader_container->shader.setMatrix4("view", context->view);
	material->shader_container->shader.setMatrix4("projection", context->projection);

	material->shader_container->shader.setFloat("shininess", 0.2f);
	material->shader_container->shader.setVec3("viewPos", context->view_pos);
	material->shader_container->shader.setInt("currentEntityID", (int)context->current_entity_id);
	
	uint num_spot_lights = 0;
	uint num_point_lights = 0;
	uint num_dir_lights = 0;
	for(int j = 0; j < context->lights.size(); ++j) {
	    LightComponent::LightType type = context->lights[j].type;

	    switch(type) {
	    case LightComponent::LightType::DIRECTIONAL: {
		std::string element = "dirLights[" + std::to_string(num_dir_lights++) + "]";
				
		std::string value = element + ".direction";
		material->shader_container->shader.setVec3(value.c_str(), GLMExtensions::modelToForward(context->light_transforms[j].getModelMatrix()));
		value = element + ".ambient";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].ambient);
		value = element + ".diffuse";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].diffuse);
		value = element + ".specular";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].specular);
	    } break;
	    case LightComponent::LightType::POINT: {
		std::string element = "pointLights[" + std::to_string(num_point_lights++) + "]";
				
		std::string value = element + ".position";
		material->shader_container->shader.setVec3(value.c_str(), context->light_transforms[j].position);
		value = element + ".ambient";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].ambient);
		value = element + ".diffuse";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].diffuse);
		value = element + ".specular";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].specular);
		value = element + ".constant";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.point_light.constant);
		value = element + ".linear";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.point_light.linear);
		value = element + ".quadratic";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.point_light.quadratic);
	    } break;
	    case LightComponent::LightType::SPOT: {
		std::string element = "spotLights[" + std::to_string(num_spot_lights++) + "]";
				
		std::string value = element + ".position";				
		material->shader_container->shader.setVec3(value.c_str(), context->light_transforms[j].position);
		value = element + ".direction";
		material->shader_container->shader.setVec3(value.c_str(), GLMExtensions::modelToForward(context->light_transforms[j].getModelMatrix()));
		value = element + ".cutOff";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.spot_light.cut_off);
		value = element + ".outerCutOff";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.spot_light.outer_cut_off);
		value = element + ".ambient";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].ambient);
		value = element + ".diffuse";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].diffuse);
		value = element + ".specular";
		material->shader_container->shader.setVec3(value.c_str(), context->lights[j].specular);
		value = element + ".constant";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.spot_light.constant);
		value = element + ".linear";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.spot_light.linear);
		value = element + ".quadratic";
		material->shader_container->shader.setFloat(value.c_str(), context->lights[j].properties.spot_light.quadratic);
	    } break;
	    };
	}

	for(auto& node : material->nodes) {
	    setMaterialNodeUniforms(material->shader_container->shader, node);
	}
		
	Mesh& target = context->mesh->meshes[i];
       	
	target.va.bind();
	glDrawElements(GL_TRIANGLES, target.indices.size(), GL_UNSIGNED_INT, 0);
	target.va.unbind();

	material->shader_container->shader.unbind();
    }
}

void Renderer::drawMeshWireframe(RendererContext *context) {
    // TODO: need to create our own shader, since mouse picking is not working correctly with
    // this kind of wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawMeshSolid(context);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::drawMeshVertices(RendererContext *context) {
    // Dont scale points with distance
    glm::vec3 camera_position = glm::inverse(context->view)[3];
    glm::vec3 object_position = context->model[3];
    float distance = glm::distance(camera_position, object_position);
    glPointSize(1/distance * 25);
    
    for(int i = 0; i < context->mesh->meshes.size(); ++i) {
	mesh_vertices_shader.bind();
	mesh_vertices_shader.setMatrix4("model", context->model);
	mesh_vertices_shader.setMatrix4("view", context->view);
	mesh_vertices_shader.setMatrix4("projection", context->projection);
	mesh_vertices_shader.setVec3("color", context->vertices_color);
	mesh_vertices_shader.setInt("currentEntityID", (int)context->current_entity_id);
	
	Mesh& target = context->mesh->meshes[i];
		
	target.va.bind();
	glDrawElements(GL_POINTS, target.indices.size(), GL_UNSIGNED_INT, 0);
	target.va.unbind();

	mesh_vertices_shader.unbind();
    }
}

void Renderer::applyFXAA(RendererContext *context) {
    fxaa_shader.bind();

    fxaa_shader.setInt("colorTexture", 0);
    fxaa_shader.setVec2("viewportSize", context->viewport_size);
    
    quad_mesh.va.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, context->color_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    quad_mesh.va.unbind();
    
    fxaa_shader.unbind();
}

void Renderer::applyMeshOutline(RendererContext *context) {
    outline_selected_mesh_shader.bind();

    outline_selected_mesh_shader.setInt("colorTexture", 0);
    outline_selected_mesh_shader.setInt("entitiesTexture", 1);
    outline_selected_mesh_shader.setInt("selectedEntityID", (int)context->selected_entity_id);
    outline_selected_mesh_shader.setVec3("selectionColor", context->outline_color);
    outline_selected_mesh_shader.setFloat("mixFactor", context->mix_factor);

    quad_mesh.va.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, context->color_texture_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, context->entities_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    quad_mesh.va.unbind();
    
    outline_selected_mesh_shader.unbind();
}

void Renderer::applyVertexOutline(RendererContext *context) {
    outline_selected_vertex_shader.bind();

    outline_selected_vertex_shader.setInt("colorTexture", 0);
    outline_selected_vertex_shader.setInt("entitiesTexture", 1);
    outline_selected_vertex_shader.setInt("selectedEntityID", (int)context->selected_entity_id);
    outline_selected_vertex_shader.setInt("selectedVertexID", (int)context->selected_vertex_id);
    outline_selected_vertex_shader.setVec3("selectionColor", context->outline_color);
    outline_selected_vertex_shader.setFloat("mixFactor", context->mix_factor);

    quad_mesh.va.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, context->color_texture_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, context->entities_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    quad_mesh.va.unbind();
    
    outline_selected_vertex_shader.unbind();
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setClearColor(const glm::vec4& color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

void Renderer::setMaterialNodeUniforms(Shader& shader, Node& node) {    
    for(auto& input : node.inputs) {
	// check if should be uniform
	if(input.links.size() == 0) {	    	   	   
	    setMaterialNodeUniform(shader, node, input);
	}
    }    
}

void Renderer::setMaterialNodeUniform(Shader& shader, Node& node, NodeProperty& prop) {    
    std::string name = std::string("input_") + std::to_string(node.id.id) + std::string("_") + std::to_string(prop.id.id);    
    switch(prop.type) {
    case NodePropertyType::COLOR:
	shader.setVec4(name.c_str(), prop.value.color_value);
	break;
    case NodePropertyType::VECTOR3:
	shader.setVec3(name.c_str(), prop.value.vector3_value);
	break;
    case NodePropertyType::VECTOR2:
	shader.setVec2(name.c_str(), prop.value.vector2_value);
	break;
    case NodePropertyType::FLOAT:
	shader.setFloat(name.c_str(), prop.value.float_value);
	break;
    case NodePropertyType::INT:
	shader.setInt(name.c_str(), prop.value.int_value);
	break;
    case NodePropertyType::ENUM:
	shader.setInt(name.c_str(), prop.value.enum_value);
	break;
    case NodePropertyType::SHADER:
	// TODO: what we should do?
	break;
    case NodePropertyType::TEXTURE:
	shader.setInt(name.c_str(), prop.value.texture_value.getID());
	break;
    default:
	printf("Unhandled node property type\n");
    }
}
