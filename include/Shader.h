#pragma once
#include "file.h"

GLuint CompileShader(const char* vsFilename, const char* fsFilename) {
	
	int success;
	char infoLog[512];

	//Compile Vertex Shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char* vertexShaderSource = read_file(vsFilename);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "Vertex Shader Compiation Fail - %s\n", infoLog);

	}

	//Compile fragment Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char* fragmentShaderSource = read_file(fsFilename);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "Fragment Shader Compiation Fail - %s\n", infoLog);

	}

	//Link Program
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "Shader Program Link Fail - %s\n", infoLog);

	}

	free(fragmentShaderSource);
	free(vertexShaderSource);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

inline void SetShaderScene(unsigned int shader,
	const glm::mat4& view,
	const glm::mat4& projection,
	const glm::mat4& lightSpaceMatrix,
	const glm::vec3& camPos,
	const glm::vec3& lightDir,
	const glm::vec3& lightColor,
	const glm::vec3& lightPos)
{
	glUseProgram(shader);
	//v,p
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	// 2. camera,light
	glUniform3f(glGetUniformLocation(shader, "lightDirection"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(shader, "directionalLightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(shader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shader, "camPos"), camPos.x, camPos.y, camPos.z);
}