#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "error.h"
#include "file.h"
#include "shader.h"
#include "camera.h"
#include "Model.h"
#include "Common.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

float camX = .0f;
float camZ = .0f;
float camY = 0.0f;

int windowWidth = 800;
int windowHeight = 600;

//light
glm::vec3 lightDirection = glm::vec3(0.2f, -.81f, .31f);
glm::vec3 lightPos = glm::vec3(2.f, 6.f, 7.f);
glm::vec3 lightColor = glm::vec3(0.8f, 0.8f, 0.8f);

SCamera Camera;

float cubeVertices[] = {
	//back face
		// positions          // col			//SPECIFY ALPHA COORDINATE HERE //nor
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,							 0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,							 0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,							 0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,							 0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,							 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,							 0.0f, 0.0f, -1.0f,

		//front face
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,							 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,							 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,							 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,							 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,							 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,							 0.0f, 0.0f, 1.0f,

		//left face
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.0f,							-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.0f,							-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.0f,							-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.0f,							-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.0f,							-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.0f,							-1.0f, 0.0f, 0.0f,

		//right face
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.0f,							1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.0f,							1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.0f,							1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.0f,							1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.0f,							1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.0f,							1.0f, 0.0f, 0.0f,

		 //bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.0f,							0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.0f,							0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.0f,							0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.0f,							0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.0f,							0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.0f,							0.0f, -1.0f, 0.0f,

		//top face
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.0f,							0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.0f,							0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.0f,							0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.0f,							0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.0f,							0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.0f,							0.0f, 1.0f, 0.0f,
};

float planeVertices[] = {
	// positions          // col			//SPECIFY ALPHA COORDINATE HERE
	 5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	-5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	-5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.0f,

	 5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	-5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	 5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.0f,
};

float transparentVertices[] = {
	// positions				// RGB				//SPECIFY ALPHA COORDINATE HERE
	-.5f,  -0.5f,  0.0f,		1.0f,  0.0f, 0.0f,	0.2f,		0.0f, 0.0f, -1.0f,
	.0f, 0.5f,  0.0f,			1.0f,  0.0f, 0.0f,	0.2f,		0.0f, 0.0f, -1.0f,
	.5f, -0.5f,  0.0f,			1.0f,  0.0f, 0.0f,	0.2f,		0.0f, 0.0f, -1.0f,
};

float transparentCubeVertices[] = {
	// positions          // col			//SPECIFY ALPHA COORDINATE HERE
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	0.4f,							0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	0.4f,							0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	0.4f,							0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	0.4f,							0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	0.4f,							0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	0.4f,							0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	0.4f,							0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	0.4f,							0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	0.4f,							0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	0.4f,							0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	0.4f,							0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	0.4f,							0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	0.4f,							-1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.f,	0.4f,							-1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	0.4f,							-1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	0.4f,							-1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.f,	0.4f,							-1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	0.4f,							-1.0f, 0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	0.4f,							1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.f,	0.4f,							1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	0.4f,							1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	0.4f,							1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.f,	0.4f,							1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	0.4f,							1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	0.4f,							0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	0.4f,							0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	0.4f,							0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	0.4f,							0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	0.4f,							0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	0.4f,							0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	0.4f,							0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	0.4f,							0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	0.4f,							0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	0.4f,							0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	0.4f,							0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	0.4f,							0.0f, 1.0f, 0.0f,
};

float quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};



void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camZ -= .01f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camZ += .01f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camX -= .01f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camX += .01f;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camY -= .01f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camY += .01f;

	//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, true);

	float x_offset = 0.f;
	float y_offset = 0.f;
	bool cam_changed = false;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		x_offset = 0.5f;
		y_offset = 0.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		x_offset = -0.5f;
		y_offset = 0.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		x_offset = 0.f;
		y_offset = -0.5f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		x_offset = 0.f;
		y_offset = 0.5f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam_dist -= 0.1f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		cam_dist += 0.1f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		lightDirection = Camera.Front;
		lightPos = Camera.Position;
	}

	if (cam_changed = true)
	{
		MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_offset, y_offset);
	}
}

void setupVertexBuffers(unsigned int* VAO, unsigned int* VBO)
{
	glGenVertexArrays(4, VAO);
	glGenBuffers(4, VBO);

	// cube
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// plane
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// transparent
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// transparent cube
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentCubeVertices), transparentCubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void setupQuadVertexBuffer(unsigned int& quadVAO, unsigned int& quadVBO)
{
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void setupDepthPeelingFramebuffer(unsigned int& FBO_0, unsigned int& colorTex_0, unsigned int& depthTex_0, unsigned int& FBO_1, unsigned int& colorTex_1, unsigned int& depthTex_1)
{
	glGenFramebuffers(1, &FBO_0);
	glGenTextures(1, &colorTex_0);
	glGenTextures(1, &depthTex_0);

	glBindTexture(GL_TEXTURE_2D, colorTex_0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, depthTex_0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex_0, 0);

	glGenFramebuffers(1, &FBO_1);
	glGenTextures(1, &colorTex_1);
	glGenTextures(1, &depthTex_1);

	glBindTexture(GL_TEXTURE_2D, colorTex_1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, depthTex_1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_1, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex_1, 0);
}

void renderScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, std::vector<glm::vec3>& positions, Model opaqueObj, Model transObj, glm::mat4 projectedLightSpaceMatrix)
{
	SetShaderScene(shaderProgram, view, projection, projectedLightSpaceMatrix, Camera.Position, lightDirection, lightColor, lightPos);

	glm::mat4 model = glm::mat4(1.f);

	// cubes
	glBindVertexArray(VAO[0]);
	model = glm::translate(model, glm::vec3(-0.8f, 0.0f, 1.0f));
	model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
	model = glm::scale(model, glm::vec3(.5, .5, .5));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, 0.0f, -1.0f));
	model = glm::rotate(model, (float)glfwGetTime() / -2.f, glm::vec3(0.0f, 1.0f, .0f));
	model = glm::scale(model, glm::vec3(.5, .5, .5));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// floor 
	glBindVertexArray(VAO[1]);
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	// transparent triangles
	glBindVertexArray(VAO[2]);
	for (size_t i = 0; i < positions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, positions[i]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	// transparent cubes
	glBindVertexArray(VAO[3]);
	model = glm::translate(model, glm::vec3(0.8f, 0.0f, -1.0f));
	model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
	model = glm::scale(model, glm::vec3(.5, .5, .5));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	if (opaqueObj.vertexCount > 0)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(opaqueObj.VAO);
		glDrawArrays(GL_TRIANGLES, 0, opaqueObj.vertexCount);
	}

	if (transObj.vertexCount > 0)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.f));
		//model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(transObj.VAO);
		glDrawArrays(GL_TRIANGLES, 0, transObj.vertexCount);
	}

}

int main(int argc, char** argv)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Depth Peeling", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	unsigned int peelShader = CompileShader("peel.vert", "peel.frag");
	unsigned int blendShader = CompileShader("blend.vert", "blend.frag");
	unsigned int finalShader = CompileShader("final.vert", "final.frag");
	unsigned int composeShader = CompileShader("compose.vert", "compose.frag");

	//camera
	InitCamera(Camera);
	cam_dist = 5.5f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);

	// setup VAO VBO
	unsigned int VAO[4], VBO[4];
	setupVertexBuffers(VAO, VBO);

	// quad for compose
	unsigned int quadVAO, quadVBO;
	setupQuadVertexBuffer(quadVAO, quadVBO);

	// resource for depth peeling:2 FBOs:2color+2depth
	// FBO_0:color用于合成最终结果,depth用来存储当前层的深度，用于剥离出下一层
	unsigned int FBO_0;
	unsigned int colorTex_0, depthTex_0;
	// FBO_1:color用来临时存剥离出的层,depth用来参考
	unsigned int FBO_1;
	unsigned int colorTex_1, depthTex_1;
	setupDepthPeelingFramebuffer(FBO_0, colorTex_0, depthTex_0, FBO_1, colorTex_1, depthTex_1);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<glm::vec3> positions;
	positions.push_back(glm::vec3(0, 0, 2.f));
	positions.push_back(glm::vec3(0.5f, 0, 0.f));
	positions.push_back(glm::vec3(0, 0, 1.f));
	positions.push_back(glm::vec3(-.5f, 0, 0.5f));
	positions.push_back(glm::vec3(0, 0, 1.5f));

	Model TransObj = loadModel("../../resource/chess-set/source/ChessSetTransparent.obj", 0.5f);
	Model OpaqueObj = loadModel("../../resource/chess-set/source/ChessSetOpaque.obj", 1.f);

	// Use query to early termination
	GLuint queryId;
	glGenQueries(1, &queryId);

	while (!glfwWindowShouldClose(window))
	{
		//V
		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		view = glm::translate(view, -glm::vec3(camX, camY, camZ));
		//P
		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)windowWidth / (float)windowHeight, .001f, 10.f);
		//light matrix
		float near_plane = 1.0f, far_plane = 70.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 projecedLightSpaceMatrix = lightProjection * lightView;

		// ========== Initial ==========
		// FBO_0: alpha=1.0, depth=0.0
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(0.0f);
		//glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// FBO_1: alpha=1.0, depth=0.0
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(0.0f);
		//glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
		int inputDepth = 0;  // 0 = depthTex_0, 1 = depthTex_1
		int outputDepth = 1;
		const int numPeels = 8;

		int currDepth = 0;  // 0 = depthTex_0, 1 = depthTex_1
		int prevDepth = 1;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);  
		glDepthMask(GL_TRUE);

		// ========== Peel loop ==========
		for (int i = 0; i < numPeels; i++)
		{
			glBeginQuery(GL_SAMPLES_PASSED, queryId);

			// clear
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, outputDepth ? depthTex_1 : depthTex_0, 0);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);  
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			
			glUseProgram(peelShader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, inputDepth ? depthTex_1 : depthTex_0);
			glUniform1i(glGetUniformLocation(peelShader, "depthTexture"), 0);

			renderScene(VAO, peelShader, view, projection, positions, OpaqueObj, TransObj, projecedLightSpaceMatrix);

			glEndQuery(GL_SAMPLES_PASSED);

			GLint available = 0;
			while (!available) {
				glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);
			}

			GLuint sampleCount = 0;
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &sampleCount);
			std::cout << "Layer " << i << " samples: " << sampleCount << std::endl;

			// ========== 混合当前层到 FBO_0 ==========
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_0);			
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendEquation();
			//
			glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);

			glUseProgram(blendShader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorTex_1);
			glUniform1i(glGetUniformLocation(blendShader, "colorTexture"), 0);

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);

			// swap depthTex
			inputDepth = (inputDepth + 1) % 2;
			outputDepth = (outputDepth + 1) % 2;

			// terminate if no samples passed
			if (sampleCount <= 0) {
				std::cout << "Early termination at layer " << i << std::endl;
				break;
			}
		}
		
		/*for (int i = 0; i < numPeels; i++)
		{
			
			//glDepthFunc(GL_LESS);

			currDepth = i % 2;//记录当前深度 0
			prevDepth = (i + 1) % 2;//传到frag里去 1

			// clear
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,colorTex_1,0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, currDepth ? depthTex_1 : depthTex_0, 0);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 开始查询
			glBeginQuery(GL_SAMPLES_PASSED, queryId);

			glUseProgram(peelShader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, prevDepth ? depthTex_1 : depthTex_0);
			glUniform1i(glGetUniformLocation(peelShader, "depthTexture"), 0);
			glUniform1i(glGetUniformLocation(peelShader, "layerIndex"), i);
			glUniform1f(glGetUniformLocation(peelShader, "windowWidth"), windowWidth);
			glUniform1f(glGetUniformLocation(peelShader, "windowHeight"), windowHeight);

			renderScene(VAO, peelShader, view, projection, positions);

			glEndQuery(GL_SAMPLES_PASSED);

			GLint available = 0;
			while (!available) {
				glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);
			}

			GLuint sampleCount = 0;
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &sampleCount);
			std::cout << "Layer " << i << " samples: " << sampleCount << std::endl;

			// ========== 混合当前层到 FBO_0 ==========
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_0);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			//
			glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(blendShader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorTex_1);
			glUniform1i(glGetUniformLocation(blendShader, "colorTexture"), 0);

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);

			// swap depthTex


			// terminate if no samples passed
			if (sampleCount <= 0) {
				std::cout << "Early termination at layer " << i << std::endl;
				break;
			}
		}
		*/
		// ========== composite to screen ==========
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glUseProgram(finalShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTex_0);
		glUniform1i(glGetUniformLocation(finalShader, "colorTexture"), 0);
		glUniform4f(glGetUniformLocation(finalShader, "backgroundColor"), 0.02f, 0.5f, 0.75f,1.0f);
		//glBindVertexArray(0);

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);
		//glUseProgram(composeShader);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, colorTex_0);
		//glUniform1i(glGetUniformLocation(composeShader, "screenTexture"), 0);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glfwSwapBuffers(window);

		showFPS(window, "DepthPeeling");

		glfwPollEvents();
		processKeyboard(window);
	}

	glDeleteVertexArrays(4, VAO);
	glDeleteBuffers(4, VBO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteFramebuffers(1, &FBO_0);
	glDeleteFramebuffers(1, &FBO_1);
	glDeleteTextures(1, &colorTex_0);
	glDeleteTextures(1, &colorTex_1);
	glDeleteTextures(1, &depthTex_0);
	glDeleteTextures(1, &depthTex_1);
	glDeleteQueries(1, &queryId);

	glfwTerminate();
	return 0;
}