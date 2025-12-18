#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <map>

#include "error.h"
#include "file.h"
#include "shader.h"
#include "Model.h"
#include "Common.h"
#include "texture.h"
#include "camera.h"
#include "Timer.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "InputController.h"
#include "Scene.h"

Scene scene;

const unsigned int MAX_FRAGMENTS = scene.windowWidth * scene.windowHeight * 16;

struct ListNode
{
	float colorR, colorG, colorB, colorA;//4*4 bytes
	//float alpha;						//4 bytes
	float depth;						//4 bytes
	unsigned int next;					//4 bytes
	unsigned int padding[2];				//8 bytes
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

void setupPPLLBuffers(unsigned int& headPtrTex, unsigned int& fragListBuffer, unsigned int& counterbuffer, unsigned int& depthTex)
{

	// 1. head pointer texture
	glGenTextures(1, &headPtrTex);
	glBindTexture(GL_TEXTURE_2D, headPtrTex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, scene.windowWidth, scene.windowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	std::vector<GLuint> initData(scene.windowWidth * scene.windowHeight, 0xFFFFFFFF);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, scene.windowWidth, scene.windowHeight, GL_RED_INTEGER, GL_UNSIGNED_INT, initData.data());

	//2. linked list buffer
	glGenBuffers(1, &fragListBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, fragListBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_FRAGMENTS * sizeof(ListNode), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fragListBuffer);

	glGenBuffers(1, &counterbuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterbuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterbuffer);

	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, scene.windowWidth, scene.windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void clearPPLLBUffers(unsigned int headPtrTex, unsigned int counterBuffer)
{

	glBindTexture(GL_TEXTURE_2D, headPtrTex);
	std::vector<GLuint> initData(scene.windowWidth * scene.windowHeight, 0xFFFFFFFF);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, scene.windowWidth, scene.windowHeight, GL_RED_INTEGER, GL_UNSIGNED_INT, initData.data());
	glBindTexture(GL_TEXTURE_2D, 0);
	// clear counter buffer
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	GLuint zero = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void renderOpaqueScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model opaqueObj, glm::mat4 projectedLightSpaceMatrix, unsigned int texture, unsigned int whiteTex)
{
	

	SetShaderScene(shaderProgram, view, projection, projectedLightSpaceMatrix, scene.camera.Position, scene.lightDirection, scene.lightColor, scene.lightPos);

	glm::mat4 model = glm::mat4(1.f);

	glActiveTexture(GL_TEXTURE0);
	if (texture > 0)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, whiteTex);
	}
	glUniform1i(glGetUniformLocation(shaderProgram, "mainTex"), 0);

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
}

void renderTransparentScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, std::vector<glm::vec3>& positions, Model transObj, glm::mat4 projectedLightSpaceMatrix, unsigned int texture, unsigned int whiteTex)
{

	SetShaderScene(shaderProgram, view, projection, projectedLightSpaceMatrix, scene.camera.Position, scene.lightDirection, scene.lightColor, scene.lightPos);

	glm::mat4 model = glm::mat4(1.f);

	glActiveTexture(GL_TEXTURE0);
	if (texture > 0)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, whiteTex);
	}
	glUniform1i(glGetUniformLocation(shaderProgram, "mainTex"), 0);

	for (int i = 0; i <= 0; i++)
	{
		for (int j = 0; j <= 0; j++)
		{
			if (transObj.vertexCount > 0)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.0f + j, -1.25f * i));
				//model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
				model = glm::scale(model, glm::vec3(0.5f));

				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(transObj.VAO);
				glDrawArrays(GL_TRIANGLES, 0, transObj.vertexCount);
			}
		}
	}
}

int main(int argc, char** argv)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(scene.windowWidth, scene.windowHeight, "A-Buffer", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	// Compile shaders
	unsigned int opaqueShader = CompileShader("opaque.vert", "opaque.frag");
	unsigned int ppllBuildShader = CompileShader("ppllBuild.vert", "ppllBuild.frag");
	unsigned int ppllResolveShader = CompileShader("ppllResolve.vert", "ppllResolve.frag");
	unsigned int composeShader = CompileShader("compose.vert", "compose.frag");
	//unsigned int debugShader = CompileShader("debug.vert", "debug.frag");
	
	// quad for compose
	unsigned int quadVAO, quadVBO;
	setupQuadVertexBuffer(quadVAO, quadVBO);

	//2 buffers for PPLL: one for storing data(color,depth,next); one for counter
	unsigned int headPtrTex;
	unsigned int fragListBuffer;
	unsigned int counterBuffer;
	unsigned int depthTex;

	setupPPLLBuffers(headPtrTex, fragListBuffer, counterBuffer, depthTex);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<glm::vec3> positions;
	positions.push_back(glm::vec3(0, 0, 2.f));
	positions.push_back(glm::vec3(0.5f, 0, 0.f));
	positions.push_back(glm::vec3(0, 0, 1.f));
	positions.push_back(glm::vec3(-.5f, 0, 0.5f));
	positions.push_back(glm::vec3(0, 0, 1.5f));

	//Model TransObj = loadModel("../../resource/chess-set/source/ChessSetTransparent.obj", 0.5f);
	//Model OpaqueObj = loadModel("../../resource/chess-set/source/ChessSetOpaque.obj", 1.f);
	//Model BoardObj = loadModel("../../resource/chess-set/source/BoardOpaque.obj", 1.f);
	//Model DragonObj = loadModel("../../resource/dragon.obj", 0.5f);
	Model MachineObj = loadModel("../../resource/machine2.obj", 0.2f);
	//GLuint BoardTex = setup_texture("../../resource/chess-set/textures/ChessboardComplete.bmp");
	//GLuint ChessTex = setup_texture("../../resource/chess-set/textures/ChessPiece.png");
	GLuint whiteTex = setup_white_texture();

	GpuTimer timer;

	while (!glfwWindowShouldClose(window))
	{
		timer.Start();
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		static const GLfloat bgd[] = { .02f, .5f, .75f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(scene.camera.Position, scene.camera.Position + scene.camera.Front, scene.camera.Up);

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)scene.windowWidth / (float)scene.windowHeight, .1f, 100.f);

		float near_plane = 1.0f, far_plane = 70.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(scene.lightPos, scene.lightPos + scene.lightDirection, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 projecedLightSpaceMatrix = lightProjection * lightView;

		// ========== Pass 1: Render opaque obj to framebuffer0 ==========
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glUseProgram(opaqueShader);
		//renderOpaqueScene(&OpaqueObj.VAO, opaqueShader, view, projection, OpaqueObj, projecedLightSpaceMatrix,ChessTex,whiteTex);
		//renderOpaqueScene(&BoardObj.VAO, opaqueShader, view, projection, BoardObj, projecedLightSpaceMatrix,BoardTex,whiteTex);

		//glBindTexture(GL_TEXTURE_2D, depthTex);
		//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,0, 0, scene.windowWidth, scene.windowHeight, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);

		// ========== Pass 2: buil ppll buffer ==========
		//depth test: true; depth writting: false
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_BLEND);

		clearPPLLBUffers(headPtrTex, counterBuffer);
		glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, fragListBuffer);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, counterBuffer);

		glUseProgram(ppllBuildShader);
		//renderTransparentScene(&TransObj.VAO, ppllBuildShader, view, projection, positions,TransObj,projecedLightSpaceMatrix,ChessTex,whiteTex);
		//renderTransparentScene(&DragonObj.VAO, ppllBuildShader, view, projection, positions,DragonObj,projecedLightSpaceMatrix,0,whiteTex);
		renderTransparentScene(&MachineObj.VAO, ppllBuildShader, view, projection, positions,MachineObj,projecedLightSpaceMatrix,0,whiteTex);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		//========== Pass 3: rennder to ppllBuffer ==========
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//--change start
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		//glDisable(GL_DEPTH_TEST);
		//--change end
		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


		glUseProgram(ppllResolveShader);
		glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, fragListBuffer);

	
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		timer.StopAndDisplay(window, "A-buffer");
		glfwSwapBuffers(window);
		glfwPollEvents();
		processKeyboard(window, scene.camera, scene.lightPos, scene.lightDirection);
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteTextures(1, &headPtrTex);
	glDeleteTextures(1, &depthTex);
	glDeleteBuffers(1, &fragListBuffer);
	glDeleteBuffers(1, &counterBuffer);
	glDeleteProgram(opaqueShader);
	glDeleteProgram(ppllBuildShader);
	glDeleteProgram(ppllResolveShader);

	glDeleteTextures(1, &depthTex);

	glfwTerminate();
	return 0;
}
