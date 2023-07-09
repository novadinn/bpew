#include "space_modeling.h"

#include "../graphics/gizmos.h"
#include "../graphics/renderer.h"
#include "../core/input.h"
#include "../core/utils.h"

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

EventReceiver *createSpaceModelingReceiver() {
    EventReceiver *receiver = new EventReceiver();

    receiver->clearEvents();

    receiver->onCreate = onCreateSpaceModeling;
    receiver->onDestroy = onDestroySpaceModeling;

    receiver->onUpdate = onUpdateSpaceModeling;
    receiver->onResize = onResizeSpaceModeling;

    receiver->onRenderBegin = onRenderBeginSpaceModeling;
    receiver->onRender = onRenderSpaceModeling;
    receiver->onRenderPostProcessing = onRenderPostProcessingSpaceModeling;
    receiver->onRenderEnd = onRenderEndSpaceModeling;

    receiver->onDrawUIBegin = onDrawUIBeginSpaceModeling;
    receiver->onDrawUI = onDrawUISpaceModeling;
    receiver->onDrawUIEnd = onDrawUIEndSpaceModeling;

    return receiver;
}

void onCreateSpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = new SpaceModelingData();    
    ctx->space_modeling_data = space_data;

    FramebufferData data;
    data.formats = {GL_RGBA8, GL_RGBA32F, GL_DEPTH24_STENCIL8};
    data.width = 800;
    data.height = 800;
    data.samples = 1;
    space_data->framebuffer.create(data);

    data.formats = {GL_RGBA8};
    space_data->pp_framebuffer.create(data);
}

void onDestroySpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;

    space_data->pp_framebuffer.destroy();
    space_data->framebuffer.destroy();
    
    delete space_data;
}

void onUpdateSpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;

    if(Input::wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
	if(space_data->viewport_hovered && !ImGuizmo::IsOver())
	    ctx->selected_entity = space_data->hovered_entity;
    }
    
    if(Input::wasKeyPressed(SDLK_q)) {
	space_data->gizmo_operation = -1;
    } else if(Input::wasKeyPressed(SDLK_t)) {
	space_data->gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
    } else if(Input::wasKeyPressed(SDLK_r)) {
	space_data->gizmo_operation = ImGuizmo::OPERATION::ROTATE;
    } else if(Input::wasKeyPressed(SDLK_s)) {
	space_data->gizmo_operation = ImGuizmo::OPERATION::SCALE;
    }

    if(Input::wasKeyHeld(SDLK_z)) {
	if(Input::wasKeyPressed(SDLK_1)) {
	    space_data->draw_mode = DrawMode::WIREFRAME;
	} else if(Input::wasKeyPressed(SDLK_2)) {
	    space_data->draw_mode = DrawMode::RENDERED;
	} else if(Input::wasKeyPressed(SDLK_3)) {
	    space_data->draw_mode = DrawMode::SOLID;
	} else if(Input::wasKeyPressed(SDLK_4)) {
	    space_data->draw_mode = DrawMode::MATERIAL_PREVIEW;
	}
    }

    
    switch(space_data->draw_mode) {
    case DrawMode::RENDERED:
	ctx->scene->onUpdateRendered();
	break;
    case DrawMode::MATERIAL_PREVIEW:
	ctx->scene->onUpdateMaterialPreview();
	break;   	
    }
}

void onResizeSpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;

    ctx->editor_camera->setViewportSize(space_data->viewport_size.x, space_data->viewport_size.y);
    ctx->scene->onResize(space_data->viewport_size.x, space_data->viewport_size.y);
    
    FramebufferData data = space_data->framebuffer.getFramebufferData();
    
    if(space_data->viewport_size.x > 0.0f && space_data->viewport_size.y > 0.0f &&
       (data.width != space_data->viewport_size.x || data.height != space_data->viewport_size.y)) {
		
	space_data->framebuffer.resize(space_data->viewport_size.x, space_data->viewport_size.y);
	space_data->pp_framebuffer.resize(space_data->viewport_size.x, space_data->viewport_size.y);
    }
}

void onRenderBeginSpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;
    
    space_data->framebuffer.bind();
    Renderer::clear();
    space_data->framebuffer.clearRGBA8ColorAttachment(1, glm::vec4(-1, 0, 0, 0));
}

void onRenderSpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;
    RendererContext *renderer_context = ctx->renderer_context;
    
    glm::mat4 view = ctx->editor_camera->getViewMatrix();
    glm::mat4 projection = ctx->editor_camera->getProjectionMatrix();
    glm::vec3 view_pos = ctx->editor_camera->position;
    glm::vec3 direction = ctx->editor_camera->getForward();

    if(ctx->active_camera) {
	auto& camera_component = ctx->active_camera.getComponent<CameraComponent>();
	auto& transform_component = ctx->active_camera.getComponent<TransformComponent>();
	view = camera_component.getViewMatrix(transform_component.position, transform_component.rotation);
	projection = camera_component.getProjectionMatrix();
	view_pos = transform_component.position;
	direction = camera_component.getForward(transform_component.rotation);
    }

    renderer_context->setCameraData(view, projection);
    renderer_context->setEditorLightData(view_pos, direction);
    
    switch(space_data->draw_mode) {
    case DrawMode::WIREFRAME: {
	ctx->scene->onDrawWireframe(renderer_context);
    } break;
    case DrawMode::RENDERED: {
	ctx->scene->onDrawRendered(renderer_context);
    } break;
    case DrawMode::SOLID: {
	ctx->scene->onDrawSolid(renderer_context);
    } break;
    case DrawMode::MATERIAL_PREVIEW: {
	ctx->scene->onDrawMaterialPreview(renderer_context);
    } break;
    }

    renderer_context->setCameraData(view, projection);
    renderer_context->setMeshVerticesData(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
					  0.8f, (uint32)ctx->selected_entity,
					  ctx->selected_vertex_id);
    ctx->scene->onDrawMeshVerticesOutlined(renderer_context);
    
    auto[mx, my] = ImGui::GetMousePos();
    mx -= space_data->viewport_bounds[0].x;
    my -= space_data->viewport_bounds[0].y;
    glm::vec2 viewport_size = space_data->viewport_bounds[1] - space_data->viewport_bounds[0];
    my = viewport_size.y - my;
    int mouse_x = (int)mx;
    int mouse_y = (int)my;
    space_data->mouse_position = {mouse_x, mouse_y};

    if (mouse_x >= 0 && mouse_y >= 0 &&
	mouse_x < (int)viewport_size.x && mouse_y < (int)viewport_size.y) {

	space_data->framebuffer.bindReadAttachment(1);
			
	glm::vec4 pixel_data = space_data->framebuffer.readRGBA8Pixel(mouse_x, mouse_y);
	int id = (int)pixel_data.x;
	if(id != -1) {
	    Entity entity;
	    entity.create((entt::entity)id, ctx->scene);
	    space_data->hovered_entity = entity;
	} else {
	    space_data->hovered_entity = {};
	}
	
	space_data->framebuffer.bindReadAttachment(0);
    }
}

void onRenderPostProcessingSpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;
    RendererContext *renderer_context = ctx->renderer_context;    

    renderer_context->setFXAAData(space_data->viewport_size,
				  space_data->framebuffer.getColorAttachmentID(0));
    
    space_data->pp_framebuffer.bind();
    Renderer::clear();
    Renderer::applyFXAA(renderer_context);
}

void onRenderEndSpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;
    
    space_data->pp_framebuffer.unbind();
}

void onDrawUIBeginSpaceModeling(EditorContext *ctx) {
    ImGui::Begin("Modeling");
}

void onDrawUISpaceModeling(EditorContext *ctx) {
    SpaceModelingData *space_data = ctx->space_modeling_data;

    space_data->viewport_size = Utils::getAvailableViewportSize();
    space_data->viewport_bounds[0] = Utils::getAvailableViewportBoundsMin();
    space_data->viewport_bounds[1] = Utils::getAvailableViewportBoundsMax();
    space_data->viewport_hovered = Utils::isViewportHovered();
    
    ImGui::Image(reinterpret_cast<void*>(space_data->pp_framebuffer.getColorAttachmentID(0)),
		 ImVec2{ space_data->viewport_size.x, space_data->viewport_size.y },
		 ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    glm::mat4 view = ctx->editor_camera->getViewMatrix();
    glm::mat4 projection = ctx->editor_camera->getProjectionMatrix();    

    if(ctx->active_camera) {
	auto& camera_component = ctx->active_camera.getComponent<CameraComponent>();
	auto& transform_component = ctx->active_camera.getComponent<TransformComponent>();
	view = camera_component.getViewMatrix(transform_component.position, transform_component.rotation);
	projection = camera_component.getProjectionMatrix();	
    }    

    /* Draw settings */
    if(ImGui::Begin("Modeling Settings")) {
	ImGui::Text("Render Mode");
	/* TODO: use icons instead of words */
	if(ImGui::Button("Wireframe")) {
	    space_data->draw_mode = DrawMode::WIREFRAME;
	}
	ImGui::SameLine();
	if(ImGui::Button("Rendered")) {
	    space_data->draw_mode = DrawMode::RENDERED;
	}
	ImGui::SameLine();
	if(ImGui::Button("Solid")) {
	    space_data->draw_mode = DrawMode::SOLID;
	}
	ImGui::SameLine();
	if(ImGui::Button("Material")) {
	    space_data->draw_mode = DrawMode::MATERIAL_PREVIEW;
	}

	ImGui::Text("Gizmos Type");
	if(ImGui::Button("Translate")) {
	    space_data->gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();
	if(ImGui::Button("Rotate")) {
	    space_data->gizmo_operation = ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();
	if(ImGui::Button("Scale")) {
	    space_data->gizmo_operation = ImGuizmo::OPERATION::SCALE;
	}
	ImGui::SameLine();
	if(ImGui::Button("None")) {
	    space_data->gizmo_operation = -1;
	}
	ImGui::SameLine();
	
	ImGui::End();
    }

    
    // Draw gizmos
    if(ctx->selected_entity && space_data->gizmo_operation != -1 &&
       ctx->selected_entity.hasComponent<TransformComponent>()) {
	TransformComponent& transform = ctx->selected_entity.getComponent<TransformComponent>();
	glm::mat4 model = transform.getModelMatrix();	
	bool snap = Input::wasKeyHeld(SDLK_LCTRL);
	
	Gizmos::drawManupilations((ImGuizmo::OPERATION)space_data->gizmo_operation,
				  glm::value_ptr(view), glm::value_ptr(projection),
				  glm::value_ptr(model), snap);
		
	if(ImGuizmo::IsUsing()) {
	    float* model_ptr = glm::value_ptr(model);
	    float scale_result[3];
	    float rotation_result[3];
	    float translation_result[3];
	    ImGuizmo::DecomposeMatrixToComponents(model_ptr, translation_result,
						  rotation_result, scale_result);

	    transform.position = {translation_result[0], translation_result[1], translation_result[2]};
	    transform.rotation = {rotation_result[0], rotation_result[1], rotation_result[2]};
	    transform.scale = {scale_result[0], scale_result[1], scale_result[2]};
	}
    }
}

void onDrawUIEndSpaceModeling(EditorContext *ctx) {
    ImGui::End();
}
