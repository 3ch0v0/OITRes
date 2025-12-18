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
#include "texture.h"
#include "Timer.h"

#include "Scene.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "InputController.h"

Scene scene;

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scene.windowWidth, scene.windowHeight, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, depthTex_0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, scene.windowWidth, scene.windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scene.windowWidth, scene.windowHeight, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, depthTex_1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, scene.windowWidth, scene.windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_1, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex_1, 0);
}

void renderScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model boardObj, Model opaqueObj, Model transObj, glm::mat4 projectedLightSpaceMatrix)
{
	SetShaderScene(shaderProgram, view, projection, projectedLightSpaceMatrix, scene.camera.Position, scene.lightDirection, scene.lightColor, scene.lightPos);

	glm::mat4 model = glm::mat4(1.f);

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
	if (boardObj.vertexCount > 0)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(boardObj.VAO);
		glDrawArrays(GL_TRIANGLES, 0, boardObj.vertexCount);
	}


	for (int i = 0; i <= 1; i++)
	{
		for (int j = 0; j <= 0; j++)
		{
			if (transObj.vertexCount > 0)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.0f + j, -(i * 1.25f)));
				//model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
				model = glm::scale(model, glm::vec3(0.5f));

				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(transObj.VAO);
				glDrawArrays(GL_TRIANGLES, 0, transObj.vertexCount);
			}
		}
	}


}

void renderOpaqueScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model obj, glm::mat4 projectedLightSpaceMatrix, unsigned int texture, unsigned int whiteTexture)
{
	SetShaderScene(shaderProgram, view, projection, projectedLightSpaceMatrix, scene.camera.Position, scene.lightDirection, scene.lightColor, scene.lightPos);

	glm::mat4 model = glm::mat4(1.f);
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
	

	if (obj.vertexCount > 0)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		glBindVertexArray(obj.VAO);
		glDrawArrays(GL_TRIANGLES, 0, obj.vertexCount);
	}
	

}

void renderTransparentScene(unsigned int* VAO, unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model transObj, glm::mat4 projectedLightSpaceMatrix, unsigned int texture, unsigned int whiteTexture)
{
	SetShaderScene(shaderProgram, view, projection, projectedLightSpaceMatrix, scene.camera.Position, scene.lightDirection, scene.lightColor, scene.lightPos);

	glm::mat4 model = glm::mat4(1.f);

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
	GLFWwindow* window = glfwCreateWindow(scene.windowWidth, scene.windowHeight, "Depth Peeling", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	unsigned int peelShader = CompileShader("peel.vert", "peel.frag");
	unsigned int blendShader = CompileShader("blend.vert", "blend.frag");
	unsigned int finalShader = CompileShader("final.vert", "final.frag");
	unsigned int composeShader = CompileShader("compose.vert", "compose.frag");

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

	//Model TransObj = loadModel("../../resource/chess-set/source/ChessSetTransparent.obj", 0.5f);
	//Model OpaqueObj = loadModel("../../resource/chess-set/source/ChessSetOpaque.obj", 1.f);
	//Model BoardObj = loadModel("../../resource/chess-set/source/BoardOpaque.obj", 1.f);

	//Model DragonObj = loadModel("../../resource/dragon.obj", 0.5f);
	Model MachineObj = loadModel("../../resource/machine2.obj", 0.2f);

	//GLuint BoardTex = setup_texture("../../resource/chess-set/textures/ChessboardComplete.bmp");
	//GLuint ChessTex = setup_texture("../../resource/chess-set/textures/ChessPiece.png");
	GLuint whiteTex = setup_white_texture();

	// Use query to early termination
	GLuint queryId;
	glGenQueries(1, &queryId);

	GpuTimer timer;

	while (!glfwWindowShouldClose(window))
	{
		//StartTiming();
		timer.Start();
		//V
		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(scene.camera.Position, scene.camera.Position + scene.camera.Front, scene.camera.Up);

		//P
		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)scene.windowWidth / (float)scene.windowHeight, .001f, 100.f);
		//light matrix
		float near_plane = 1.0f, far_plane = 70.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(scene.lightPos, scene.lightPos + scene.lightDirection, glm::vec3(0.f, 1.f, 0.f));
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
		const int numPeels = 32;

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

			//renderOpaqueScene(& BoardObj.VAO, peelShader, view, projection,  BoardObj,  projecedLightSpaceMatrix,BoardTex,whiteTex);
			//renderOpaqueScene(& OpaqueObj.VAO, peelShader, view, projection,  OpaqueObj,  projecedLightSpaceMatrix, ChessTex, whiteTex);

			//renderTransparentScene(& TransObj.VAO, peelShader, view, projection, TransObj, projecedLightSpaceMatrix, ChessTex, whiteTex);
			//renderTransparentScene(& DragonObj.VAO, peelShader, view, projection, DragonObj, projecedLightSpaceMatrix, 0, whiteTex);
			renderTransparentScene(& MachineObj.VAO, peelShader, view, projection,MachineObj, projecedLightSpaceMatrix, 0, whiteTex);


			glEndQuery(GL_SAMPLES_PASSED);

			GLint available = 0;
			while (!available) {
				glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);
			}

			GLuint sampleCount = 0;
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &sampleCount);

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
		timer.StopAndDisplay(window, "DepthPeeling");

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window, scene.camera, scene.lightPos, scene.lightDirection);
	}

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