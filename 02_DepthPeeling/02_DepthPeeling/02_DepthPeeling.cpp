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

float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.0f,

	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.0f,

	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.0f,
};

float planeVertices[] = {
	 5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	-5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	-5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.0f,

	 5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	-5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.0f,
	 5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.0f,
};

float transparentVertices[] = {
	-.5f,  -0.5f,  0.0f,		1.0f,  0.0f, 0.0f,	0.5f,
	.0f, 0.5f,  0.0f,			1.0f,  0.0f, 0.0f,	0.5f,
	.5f, -0.5f,  0.0f,			1.0f,  0.0f, 0.0f,	0.5f,
};

float quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

float camX = .0f;
float camZ = .0f;

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
}

void renderScene(unsigned int* VAO, unsigned int shaderProgram,
	glm::mat4 view, glm::mat4 projection,
	std::vector<glm::vec3>& positions)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1.f);

	// cubes (不透明)
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

	// floor (不透明)
	glBindVertexArray(VAO[1]);
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// transparent triangles
	glBindVertexArray(VAO[2]);
	for (size_t i = 0; i < positions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, positions[i]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}

int main(int argc, char** argv)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "Depth Peeling", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	// Compile shaders
	unsigned int renderShader = CompileShader("depthpeeling.vert", "depthpeeling.frag");
	unsigned int blendShader = CompileShader("blend.vert", "blend.frag");
	unsigned int finalShader = CompileShader("final.vert", "final.frag");

	// Setup geometry VAOs
	unsigned int VAO[3];
	glGenVertexArrays(3, VAO);
	unsigned int VBO[3];
	glGenBuffers(3, VBO);

	// cube
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// quad for blending
	unsigned int quadVAO, quadVBO;
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

	// FBO_0: 累积缓冲 (最终结果)
	unsigned int FBO_0;
	glGenFramebuffers(1, &FBO_0);
	unsigned int colorTex_0, depthTex_0;
	glGenTextures(1, &colorTex_0);
	glGenTextures(1, &depthTex_0);

	glBindTexture(GL_TEXTURE_2D, colorTex_0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, depthTex_0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex_0, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO_0 not complete!" << std::endl;

	// FBO_1: Ping-pong 缓冲 (临时渲染)
	unsigned int FBO_1;
	glGenFramebuffers(1, &FBO_1);
	unsigned int colorTex_1, depthTex_1;
	glGenTextures(1, &colorTex_1);
	glGenTextures(1, &depthTex_1);

	glBindTexture(GL_TEXTURE_2D, colorTex_1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, depthTex_1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_1, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex_1, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO_1 not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<glm::vec3> positions;
	positions.push_back(glm::vec3(0, 0, 2.f));
	positions.push_back(glm::vec3(0.5f, 0, 0.f));
	positions.push_back(glm::vec3(0, 0, 1.f));
	positions.push_back(glm::vec3(-.5f, 0, 0.5f));
	positions.push_back(glm::vec3(0, 0, 1.5f));

	// Query for early termination
	GLuint queryId;
	glGenQueries(1, &queryId);

	while (!glfwWindowShouldClose(window))
	{
		processKeyboard(window);

		glm::mat4 view = glm::mat4(1.f);
		view = glm::translate(view, -glm::vec3(camX, 0.f, camZ));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)800 / (float)600, .001f, 10.f);

		// ========== 初始化阶段 ==========
		// FBO_0: alpha=1.0, depth=0.0
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// FBO_1: alpha=1.0, depth=0.0
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ========== 深度剥离循环 ==========
		int inputDepth = 0;  // 0 = depthTex_0, 1 = depthTex_1
		int outputDepth = 1;
		const int MAX_LAYERS = 10;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);  // 使用标准深度测试
		glDepthMask(GL_TRUE);

		for (int layer = 0; layer < MAX_LAYERS; layer++)
		{
			// 清空输出FBO (FBO_1)
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_1);
			// 重新绑定输出深度纹理
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
				outputDepth ? depthTex_1 : depthTex_0, 0);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // alpha = 0
			glClearDepth(1.0f);  // 深度 = 1.0
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 开始查询
			glBeginQuery(GL_SAMPLES_PASSED, queryId);

			// 渲染场景，使用输入深度纹理进行剥离
			glUseProgram(renderShader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, inputDepth ? depthTex_1 : depthTex_0);
			glUniform1i(glGetUniformLocation(renderShader, "depthTexture"), 0);

			renderScene(VAO, renderShader, view, projection, positions);

			glEndQuery(GL_SAMPLES_PASSED);

			// 等待查询结果
			GLint available = 0;
			while (!available) {
				glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);
			}

			GLuint sampleCount = 0;
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &sampleCount);
			std::cout << "Layer " << layer << " samples: " << sampleCount << std::endl;

			// ========== 混合当前层到 FBO_0 ==========
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_0);
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(blendShader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorTex_1);
			glUniform1i(glGetUniformLocation(blendShader, "colorTexture"), 0);

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);

			// 交换输入输出
			inputDepth = (inputDepth + 1) % 2;
			outputDepth = (outputDepth + 1) % 2;

			// 提前终止
			if (sampleCount <= 0) {
				std::cout << "Early termination at layer " << layer << std::endl;
				break;
			}
		}

		// ========== 最终合成到屏幕 ==========
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glUseProgram(finalShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTex_0);
		glUniform1i(glGetUniformLocation(finalShader, "colorTexture"), 0);
		glUniform3f(glGetUniformLocation(finalShader, "backgroundColor"), 0.02f, 0.5f, 0.75f);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(3, VAO);
	glDeleteBuffers(3, VBO);
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