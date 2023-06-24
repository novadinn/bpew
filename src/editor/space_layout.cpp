#include "space_layout.h"

#include "../graphics/gizmos.h"
#include "../graphics/renderer.h"
#include "../core/input.h"
#include "../core/utils.h"

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

EventReceiver *createSpaceLayoutReceiver() {
    EventReceiver *receiver = new EventReceiver();

    receiver->clearEvents();

    receiver->onCreate = onCreateSpaceLayout;
    receiver->onDestroy = onDestroySpaceLayout;

    receiver->onUpdate = onUpdateSpaceLayout;
    receiver->onResize = onResizeSpaceLayout;

    receiver->onRenderBegin = onRenderBeginSpaceLayout;
    receiver->onRender = onRenderSpaceLayout;
    receiver->onRenderPostProcessing = onRenderPostProcessingSpaceLayout;
    receiver->onRenderEnd = onRenderEndSpaceLayout;

    receiver->onDrawUIBegin = onDrawUIBeginSpaceLayout;
    receiver->onDrawUI = onDrawUISpaceLayout;
    receiver->onDrawUIEnd = onDrawUIEndSpaceLayout;

    return receiver;
}

void onCreateSpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = new SpaceLayoutData();    
    ctx->space_layout_data = space_data;

    FramebufferData data;
    data.formats = {GL_RGBA8, GL_RGBA32F, GL_DEPTH24_STENCIL8};
    data.width = 800;
    data.height = 800;
    data.samples = 1;
    space_data->framebuffer.create(data);

    data.formats = {GL_RGBA8};
    space_data->pp_framebuffer.create(data);
}

void onDestroySpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;

    space_data->pp_framebuffer.destroy();
    space_data->framebuffer.destroy();
    
    delete space_data;
}

void onUpdateSpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;

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

    // TODO: create imgui buttons instead of this
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
}

void onResizeSpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;

    ctx->scene->onResize(space_data->viewport_size.x, space_data->viewport_size.y);

    FramebufferData data = space_data->framebuffer.getFramebufferData();
    
    if(space_data->viewport_size.x > 0.0f && space_data->viewport_size.y > 0.0f &&
       (data.width != space_data->viewport_size.x || data.height != space_data->viewport_size.y)) {
		
	space_data->framebuffer.resize(space_data->viewport_size.x, space_data->viewport_size.y);
	space_data->pp_framebuffer.resize(space_data->viewport_size.x, space_data->viewport_size.y);
    }
}

void onRenderBeginSpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;
    
    space_data->framebuffer.bind();
    Renderer::clear();
    space_data->framebuffer.clearRGBA8ColorAttachment(1, glm::vec4(-1, 0, 0, 0));
}

void onRenderSpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;
    
    switch(space_data->draw_mode) {
    case DrawMode::WIREFRAME: {
	ctx->scene->onDrawWireframe();
    } break;
    case DrawMode::RENDERED: {
	ctx->scene->onDrawRendered();
    } break;
    case DrawMode::SOLID: {
	ctx->scene->onDrawSolid();
    } break;
    case DrawMode::MATERIAL_PREVIEW: {
	ctx->scene->onDrawMaterialPreview();
    } break;
    }

    auto[mx, my] = ImGui::GetMousePos();
    mx -= space_data->viewport_bounds[0].x;
    my -= space_data->viewport_bounds[0].y;
    glm::vec2 viewport_size = space_data->viewport_bounds[1] - space_data->viewport_bounds[0];
    my = viewport_size.y - my;
    int mouse_x = (int)mx;
    int mouse_y = (int)my;

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

void onRenderPostProcessingSpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;
    
    space_data->pp_framebuffer.bind();
    Renderer::clear();
    Renderer::outlineSelectedMesh(space_data->framebuffer.getColorAttachmentID(0),
				  space_data->framebuffer.getColorAttachmentID(1),
				  (uint32)ctx->selected_entity,
				  glm::vec3(0.0f, 0.0f, 1.0f), 0.5f);

}

void onRenderEndSpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;
    
    space_data->pp_framebuffer.unbind();
}

void onDrawUIBeginSpaceLayout(EditorContext *ctx) {
    ImGui::Begin("Layout");
}

void onDrawUISpaceLayout(EditorContext *ctx) {
    SpaceLayoutData *space_data = ctx->space_layout_data;

    space_data->viewport_size = Utils::getAvailableViewportSize();
    space_data->viewport_bounds[0] = Utils::getAvailableViewportBoundsMin();
    space_data->viewport_bounds[1] = Utils::getAvailableViewportBoundsMax();
    space_data->viewport_hovered = Utils::isViewportHovered();
    
    ImGui::Image(reinterpret_cast<void*>(space_data->pp_framebuffer.getColorAttachmentID(0)),
		 ImVec2{ space_data->viewport_size.x, space_data->viewport_size.y },
		 ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    // Draw gizmos
    if(ctx->selected_entity && space_data->gizmo_operation != -1 &&
       ctx->selected_entity.hasComponent<TransformComponent>()) {
	TransformComponent& transform = ctx->selected_entity.getComponent<TransformComponent>();
	glm::mat4 model = transform.getModelMatrix();
	glm::mat4 view = ctx->main_camera->getViewMatrix();
	glm::mat4 projection = ctx->main_camera->getProjectionMatrix();

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

void onDrawUIEndSpaceLayout(EditorContext *ctx) {
    ImGui::End();
}
