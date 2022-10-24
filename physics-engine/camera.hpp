#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const float g_sensitivity = 0.1f;
const float g_yaw = -90.0f;
const float g_pitch = 0.0f;
const float g_fov = 70.0f;
const float g_speed = 0.0125f;

using Direction =
enum {
	NoDir,
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

class cam {
public:
	// Require initilization from constructor
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 worldup;
	glm::vec3 right;

	float yaw;
	float pitch;

	float speed;
	float sensitivity;
	float fov;

	bool noclip = true;

	// Other members not initialized immediately
	glm::vec3 velocity;
	glm::vec3 force;

	cam(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = g_yaw, float _pitch = g_pitch)
		: front(glm::vec3(0.0f, 0.0f, -1.0f)), velocity(glm::vec3(0.0f)), force(glm::vec3(0.0f)), speed(g_speed), sensitivity(g_sensitivity), fov(g_fov) {
		position = _position;
		worldup = _up;
		yaw = _yaw;
		pitch = _pitch;

		update();
	}

	// Scalar values
	cam(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch) {
		position = glm::vec3(posX, posY, posZ);
		worldup = glm::vec3(upX, upY, upZ);
		yaw = _yaw;
		pitch = _pitch;

		update();
	}

	glm::mat4 get_matview() {
		return glm::lookAt(position, position + front, up);
	}

	void process_input(Direction direction, float time) {
		float vel = speed;
		switch (direction) {
		case Direction::Forward:
			this->force += front * speed;
			break;
		case Direction::Backward:
			this->force -= front * speed;
			break;
		case Direction::Left:
			this->force -= right * speed;
			break;
		case Direction::Right:
			this->force += right * speed;
			break;
		}
	}

	void process_mouse(float xo, float yo) {
		bool constrain_pitch = true;

		xo *= sensitivity;
		yo *= sensitivity;

		yaw += xo;
		pitch += yo;

		// Prevent screen flipping around pitch (updown)
		if (constrain_pitch) {
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		update();
	}
private:
	void update() {
		glm::vec3 newfront;
		newfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newfront.y = sin(glm::radians(pitch));
		newfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		front = glm::normalize(newfront);

		// Recalculuate right and up vectors
		right = glm::normalize(glm::cross(front, worldup));
		up = glm::normalize(glm::cross(right, front));
	}
};