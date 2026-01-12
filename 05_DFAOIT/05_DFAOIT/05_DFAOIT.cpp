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
#include "texture.h"
#include "Timer.h"
#include "InputController.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Scene.h"

Scene scene;

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

void setupDFAOITFramebuffer(unsigned int& dfaoitFBO, unsigned int& colorTex, unsigned int& countTex, unsigned int& depthTex)
{
	glGenFramebuffers(1, &dfaoitFBO);
	glGenTextures(1, &colorTex);
	glGenTextures(1, &countTex);
	//fragCounterTex:RGB32ui; accTex:RGBA16f; averageTex:RGBA16f; bucketTex:RGBA16f
	// 
	//RGB:sum of RGBA
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, scene.windowWidth, scene.windowHeight, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//R:aplha^2;G:Frragment count
	glBindTexture(GL_TEXTURE_2D, countTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, scene.windowWidth, scene.windowHeight, 0, GL_RG, GL_HALF_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, scene.windowWidth, scene.windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, dfaoitFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, countTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: WBOIT Framebuffer is not complete!" << std::endl;
}

void setupDFAOITTextures(unsigned int& fragmentCounterTex, unsigned int& accTex, unsigned int& averageTex, unsigned int& bucketTex,int w, int h)
{


	glGenTextures(1, &fragmentCounterTex);
	glBindTexture(GL_TEXTURE_2D, fragmentCounterTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, w, h, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &accTex);
	glBindTexture(GL_TEXTURE_2D, accTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &averageTex);
	glBindTexture(GL_TEXTURE_2D, averageTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &bucketTex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, bucketTex);	
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, w, h, 2, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

}

void renderOpaqueScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model opaqueObj, glm::mat4 projectedLightSpaceMatrix, unsigned int texture, unsigned int whiteTexture)
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
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
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

void renderTransparentScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model transObj, glm::mat4 projectedLightSpaceMatrix, unsigned int texture, unsigned int whiteTexture)
{
	SetShaderScene(shaderProgram, view, projection, projectedLightSpaceMatrix, scene.camera.Position, scene.lightDirection, scene.lightColor, scene.lightPos);

	glm::mat4 model = glm::mat4(1.f);
	model = glm::scale(model, glm::vec3(0.5f));
	glActiveTexture(GL_TEXTURE1);
	if (texture > 0)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
	}
	glUniform1i(glGetUniformLocation(shaderProgram, "mainTex"), 1);

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
	GLFWwindow* window = glfwCreateWindow(scene.windowWidth, scene.windowHeight, "DFAOIT", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	// Compile shaders
	unsigned int opaqueShader = CompileShader("opaque.vert", "opaque.frag");
	unsigned int dfaoitShader = CompileShader("dfaoitAccum.vert", "dfaoitAccum.frag");
	unsigned int composeShader = CompileShader("compose.vert", "compose.frag");
	//unsigned int debugShader = CompileShader("debug.vert", "debug.frag");

	// quad for compose
	unsigned int quadVAO, quadVBO;
	setupQuadVertexBuffer(quadVAO, quadVBO);

	// WBOIT FBO：1. one colorT for accumulate color; 2.another for accummulate alpha 3. one depthT 
	//unsigned int dfaoitFBO;
	//unsigned int colorTex, countTex, depthTex;
	//setupDFAOITFramebuffer(dfaoitFBO, colorTex, countTex, depthTex);

	GLuint fragmentCounterTex;
	GLuint accTex;
	GLuint averageTex;
	GLuint bucketTex;
	setupDFAOITTextures(fragmentCounterTex, accTex, averageTex, bucketTex,scene.windowWidth, scene.windowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Model TransObj = loadModel("../../resource/chess-set/source/ChessSetTransparent.obj", 0.5f);
	//Model OpaqueObj = loadModel("../../resource/chess-set/source/ChessSetOpaque.obj", 1.f);
	//Model BoardObj = loadModel("../../resource/chess-set/source/BoardOpaque.obj", 1.f);

	//Model DragonObj = loadModel("../../resource/dragon.obj", 0.5f);
	Model MachineObj = loadModel("../../resource/machine2.obj", 0.2f);

	//GLuint BoardTex = setup_texture("../../resource/chess-set/textures/ChessboardComplete.bmp");
	//GLuint ChessTex = setup_texture("../../resource/chess-set/textures/ChessPiece.png");
	GLuint whiteTex = setup_white_texture();

	//InitTimer();
	GpuTimer timer;

	while (!glfwWindowShouldClose(window))
	{
		timer.Start();
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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);


		glUseProgram(opaqueShader);
		//renderOpaqueScene(&OpaqueObj.VAO, opaqueShader, view, projection, OpaqueObj, projecedLightSpaceMatrix, ChessTex, whiteTex);
		//renderOpaqueScene(&BoardObj.VAO, opaqueShader, view, projection, BoardObj, projecedLightSpaceMatrix, BoardTex, whiteTex);



		// ========== Pass 2: Render transparent obj to DFAOIT buffer ==========
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//depth test: less; depth writting: false
		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
		glDepthMask(GL_FALSE);
		//glDisable(GL_CULL_FACE);

		//blend mode
		glDisable(GL_BLEND);

		//initial: colorTex to 0;countTex to 1
		//glBindFramebuffer(GL_FRAMEBUFFER, dfaoitFBO);
		//float clearColor0[4] = { 0.f, 0.f, 0.f, 0.f };
		//glClearBufferfv(GL_COLOR, 0, clearColor0);
		//glClearBufferfv(GL_COLOR, 1, clearColor0);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dfaoitFBO);
		//glBlitFramebuffer(0, 0, scene.windowWidth, scene.windowHeight, 0, 0, scene.windowWidth, scene.windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		//glBindFramebuffer(GL_FRAMEBUFFER, dfaoitFBO);

		glUseProgram(dfaoitShader);
		
		glBindImageTexture(0, fragmentCounterTex,0,GL_FALSE, 0,GL_READ_WRITE, GL_RGBA32UI);
		glBindImageTexture(1, accTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindImageTexture(2, averageTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindImageTexture(3, bucketTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);

		unsigned int clearCounter[4] = { 0,0x7F7FFFFF, 0x7F7FFFFF, 0 };
		glClearTexImage(fragmentCounterTex, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, clearCounter);
		float clearColor[4] = { 0.f,0.f,0.f,0.f };
		glClearTexImage(accTex, 0, GL_RGBA, GL_FLOAT, clearColor);
		glClearTexImage(averageTex, 0, GL_RGBA, GL_FLOAT, clearColor);

		//---- render loaded transparent model
		//renderTransparentScene(&TransObj.VAO, dfaoitShader, view, projection, TransObj, projecedLightSpaceMatrix, ChessTex, whiteTex);
		//renderTransparentScene(&DragonObj.VAO, dfaoitShader, view, projection, DragonObj, projecedLightSpaceMatrix,0,whiteTex);
		renderTransparentScene(&MachineObj.VAO, dfaoitShader, view, projection, MachineObj, projecedLightSpaceMatrix, 0, whiteTex);
		
		//----render loaded transparent model
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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
		//glBindTexture(GL_TEXTURE_2D, colorTex);
		//glUniform1i(glGetUniformLocation(composeShader, "colorTex"), 0);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, countTex);
		//glUniform1i(glGetUniformLocation(composeShader, "countTex"), 1);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);


		timer.StopAndDisplay(window, "DFAOIT");
		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window, scene.camera, scene.lightPos, scene.lightDirection);
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	//glDeleteFramebuffers(1, &dfaoitFBO);
	//glDeleteTextures(1, &colorTex);
	//glDeleteTextures(1, &countTex);
	//glDeleteTextures(1, &depthTex);


	glfwTerminate();
	return 0;
}
