#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vCol;
layout (location = 2) in vec3 vNor;

out vec4 col;
out vec3 nor;
out vec3 FragPosWorldSpace;
//out vec4 FragPosProjectedLightSpace;
out float fragDepth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projectedLightSpaceMatrix;

void main()
{
	gl_Position = projection * view * model * vPos;
	col = vCol;
	nor = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model * vPos);
	//FragPosProjectedLightSpace = projectedLightSpaceMatrix * model * vPos;
	fragDepth = gl_Position.z / gl_Position.w;
	vec4 viewPos = view * model * vPos;
	//fragDepth = -viewPos.z;
	//col = vec4(1,1,1,vCol.w);
}