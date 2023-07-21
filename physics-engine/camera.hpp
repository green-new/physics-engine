#pragma once
#include "orientation.hpp"
#include "transform.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Components {
	struct Camera {
		glm::mat4 Projection;
		double Pitch;
		double Yaw;

		void update(Orientation& orientation) const {
			glm::vec3 newFront = glm::vec3(0.0f);
			newFront.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			newFront.y = sin(glm::radians(Pitch));
			newFront.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			orientation.Front = glm::normalize(newFront);

			orientation.Right = glm::normalize(glm::cross(orientation.Front, glm::vec3(0.0f, 1.0f, 0.0f)));
			orientation.Up = glm::normalize(glm::cross(orientation.Right, orientation.Front));
		}
		glm::mat4 getView(const Orientation& orientation, const Transform& transform) const {
			return glm::lookAt(transform.Position, transform.Position + orientation.Front, orientation.Up);
		}
		inline static glm::mat4 createProjection(float fov, float zNear, float zFar, float width, float height) {
			return glm::perspective(fov, width / height, zNear, zFar);
		}
	};
}