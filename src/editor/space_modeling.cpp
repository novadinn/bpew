#include "space_modeling.h"

#include "../graphics/gizmos.h"

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

void SpaceModeling::begin() {
    ImGui::Begin("Modeling");
}

void SpaceModeling::drawImage(uint texture_attachment_id, const glm::vec2& viewport_size) {	
    ImGui::Image(reinterpret_cast<void*>(texture_attachment_id),
		 ImVec2{ viewport_size.x, viewport_size.y },
		 ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
}

void SpaceModeling::manipulateSelected(TransformComponent& transform,
				     glm::mat4& view, glm::mat4& projection,
				     int gizmo_operation, bool snap) {
    glm::mat4 model = transform.getModelMatrix();

    Gizmos::drawManupilations((ImGuizmo::OPERATION)gizmo_operation,
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

void SpaceModeling::end() {
    ImGui::End();
}
