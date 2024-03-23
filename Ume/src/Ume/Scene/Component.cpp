#include "umepch.h"
#include "Component.h"
#include "glm/gtx/euler_angles.hpp"

void Ume::Transform::UpdateTransform()
{
	ModelMatrix = glm::translate(glm::mat4(1.0f), Position)
		* glm::eulerAngleYXZ(glm::radians(EulerRotation.y), glm::radians(EulerRotation.x), glm::radians(EulerRotation.z))
		* glm::scale(glm::mat4(1.0f), Scale);
	NormalMatrix = glm::transpose(glm::inverse(glm::mat3(ModelMatrix)));
}
