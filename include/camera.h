#pragma once

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	glm::vec3 Target;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float CamDistance;

	SCamera()
	{
		Front = glm::vec3(0.0f, 0.0f, -1.0f);
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Up = glm::vec3(0.0f, 1.0f, 0.0f);
		WorldUp = Up;
		Right = glm::normalize(glm::cross(Front, WorldUp));

		Yaw = 45.f;
		Pitch = 2.f;
		MouseSensitivity = 1.f;
		MovementSpeed = 5.5f;
		CamDistance = 6.f;

		UpdateCamPosition();
	}

public:
	void UpdateCamPosition()
	{
		float yawRad = glm::radians(Yaw);
		float pitchRad = glm::radians(Pitch);

		Position.x = Target.x + CamDistance * cos(yawRad) * cos(pitchRad);
		Position.y = Target.y + CamDistance * sin(pitchRad);
		Position.z = Target.z + CamDistance * sin(yawRad) * cos(pitchRad);

		Front = glm::normalize(Target - Position);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}

	void Rotate(float xoffset, float yoffset)
	{
		Yaw += xoffset;
		Pitch += yoffset;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		UpdateCamPosition();
	}

	void Pan(float x, float y, float z)
	{
		Target.x += x;
		Target.y += y;
		Target.z += z;
		UpdateCamPosition();
	}

	void Zoom(float offset)
	{
		CamDistance += offset;
		if (CamDistance < 1.0f)
			CamDistance = 1.0f;
		UpdateCamPosition();
	}
	

};




