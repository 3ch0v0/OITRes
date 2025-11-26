#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "error.h"
#include "file.h"
#include "shader.h"
#include "camera.h"
#include "Model.h"
#include "Common.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

float camX = .0f;
float camZ = 0.f;
float camY = 0.0f;

const unsigned int windowWidth = 800;
const unsigned int windowHeight = 600;

//light direction variable here
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
	// positions   // texCoords
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

void setupWBOITFramebuffer(unsigned int& wboitFBO, unsigned int& colorTex, unsigned int& alphaTex, unsigned int& depthTex)
{
	glGenFramebuffers(1, &wboitFBO);
	glGenTextures(1, &colorTex);
	glGenTextures(1, &alphaTex);

	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, alphaTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, windowWidth, windowHeight, 0, GL_RED, GL_HALF_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, wboitFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, alphaTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: WBOIT Framebuffer is not complete!" << std::endl;
}

void renderOpaqueScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection,Model opaqueObj, glm::mat4 projectedLightSpaceMatrix)
{
	
	
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	//light direction
	glUniform3f(glGetUniformLocation(shaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	//light color
	glUniform3f(glGetUniformLocation(shaderProgram, "directionalLightColor"), lightColor.x, lightColor.y, lightColor.z);

	//camera pos
	glUniform3f(glGetUniformLocation(shaderProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
	//light Pos
	glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 model = glm::mat4(1.f);

	// cubes
	glBindVertexArray(VAO[0]);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
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

	if (opaqueObj.vertexCount > 0)
	{
		//glUniformMatrix4fv(glGetUniformLocation(opaqueShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(glGetUniformLocation(opaqueShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(opaqueObj.VAO);
		glDrawArrays(GL_TRIANGLES, 0, opaqueObj.vertexCount);
	}
}

void renderTransparentScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, std::vector<glm::vec3>& positions, Model transObj, glm::mat4 projectedLightSpaceMatrix)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	//light direction
	glUniform3f(glGetUniformLocation(shaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	//light color
	glUniform3f(glGetUniformLocation(shaderProgram, "directionalLightColor"), lightColor.x, lightColor.y, lightColor.z);

	//camera pos
	glUniform3f(glGetUniformLocation(shaderProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
	//light Pos
	glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 model = glm::mat4(1.f);

	// transparent triangles
	glBindVertexArray(VAO[2]);
	for (size_t i = 0; i < positions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, positions[i]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	// transparent cube
	glBindVertexArray(VAO[3]);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.8f, 0.0f, 1.0f));
	model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
	model = glm::scale(model, glm::vec3(.5, .5, .5));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

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
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "WBOIT", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	// Compile shaders
	unsigned int opaqueShader = CompileShader("opaque.vert", "opaque.frag");
	unsigned int wboitShader = CompileShader("wboit.vert", "wboit.frag");
	unsigned int composeShader = CompileShader("compose.vert", "compose.frag");
	//unsigned int debugShader = CompileShader("debug.vert", "debug.frag");

	InitCamera(Camera);
	cam_dist = 5.5f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);

	// setup VAO VBO
	unsigned int VAO[4],VBO[4];
	setupVertexBuffers(VAO, VBO);

	// quad for compose
	unsigned int quadVAO, quadVBO;
	setupQuadVertexBuffer(quadVAO, quadVBO);

	// WBOIT FBO：1. one colorT for accumulate color; 2.another for accummulate alpha 3. one depthT 
	unsigned int wboitFBO;
	unsigned int colorTex, alphaTex, depthTex;
	setupWBOITFramebuffer(wboitFBO, colorTex, alphaTex, depthTex);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<glm::vec3> positions;
	positions.push_back(glm::vec3(0, 0, 2.f));
	positions.push_back(glm::vec3(0.5f, 0, 0.f));
	positions.push_back(glm::vec3(0, 0, 1.f));
	positions.push_back(glm::vec3(-.5f, 0, 0.5f));
	positions.push_back(glm::vec3(0, 0, 1.5f));

	Model TransObj = loadModel("../../resource/chess-set/source/ChessSetTransparent.obj", 0.5f);
	Model OpaqueObj = loadModel("../../resource/chess-set/source/ChessSetOpaque.obj", 1.f);

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { .02f, .5f, .75f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glm::mat4 view = glm::mat4(1.f);
		//Camera.Position += glm::vec3(camX, camY, camZ);
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		view = glm::translate(view, -glm::vec3(camX, camY, camZ));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)windowWidth / (float)windowHeight, .1f, 100.f);

		float near_plane = 1.0f, far_plane = 70.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 projecedLightSpaceMatrix = lightProjection * lightView;

		// ========== Pass 1: Render opaque obj to framebuffer0 ==========
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);


		glUseProgram(opaqueShader);
		renderOpaqueScene(VAO, opaqueShader, view, projection, OpaqueObj, projecedLightSpaceMatrix);
		

		// ========== Pass 2: Render transparent obj to wboit buffer ==========
		//depth test: less; depth writting: false
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_FALSE);
		//glDisable(GL_CULL_FACE);

		//blend mode
		glEnable(GL_BLEND);
		glBlendFunci(0, GL_ONE, GL_ONE);
		glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
		

		//initial: colorTex to 0;alphaTex to 1
		glBindFramebuffer(GL_FRAMEBUFFER, wboitFBO);
		float clearColor0[4] = { 0.f, 0.f, 0.f, 0.f };
		float clearColor1[4] = { 1.f, 1.f, 1.f, 1.f };	
		glClearBufferfv(GL_COLOR, 0, clearColor0);
		glClearBufferfv(GL_COLOR, 1, clearColor1);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, wboitFBO);
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);//
		glBindFramebuffer(GL_FRAMEBUFFER, wboitFBO);

		glUseProgram(wboitShader);
		renderTransparentScene(VAO, wboitShader, view, projection, positions, TransObj, projecedLightSpaceMatrix);
		//---- render loaded transparent model
		
		//----render loaded transparent model
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		// ========== Pass 3: compose==========
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
		//glDisable(GL_CULL_FACE);

		glUseProgram(composeShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTex);
		glUniform1i(glGetUniformLocation(composeShader, "colorTex"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, alphaTex);
		glUniform1i(glGetUniformLocation(composeShader, "alphaTex"), 1);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);

		showFPS(window, "WBOIT");

		glfwPollEvents();
		processKeyboard(window);
	}

	glDeleteVertexArrays(4, VAO);
	glDeleteBuffers(4, VBO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteFramebuffers(1, &wboitFBO);
	glDeleteTextures(1, &colorTex);
	glDeleteTextures(1, &alphaTex);
	glDeleteTextures(1, &depthTex);


	glfwTerminate();
	return 0;
}
