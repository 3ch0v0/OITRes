#pragma once

#include <iostream>
#include <GL/gl3w.h> 
#include "stb_image.h"

GLuint setup_texture(const char* filename)
{

	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int w, h, chan;

	unsigned char* pxls = stbi_load(filename, &w, &h, &chan, 0);

	if (pxls)
	{ 
		GLenum format;
		if (chan == 1)
			format = GL_RED;
		else if (chan == 3)
			format = GL_RGB;
		else if (chan == 4)
			format = GL_RGBA;
		else
			format = GL_RGB; 

	
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pxls);

		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "Texture loaded: " << filename << " (" << w << "x" << h << ", " << chan << "ch)" << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture: " << filename << std::endl;
	}


	if (pxls) stbi_image_free(pxls);


	glBindTexture(GL_TEXTURE_2D, 0);

	return texObject;
}


GLuint setup_mipmaps(const char* filename[], int n)
{
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 

	stbi_set_flip_vertically_on_load(true);

	for (int c = 0; c < n; c++)
	{
		int w, h, chan;
		unsigned char* pxls = stbi_load(filename[c], &w, &h, &chan, 0);
		if (pxls)
		{
			GLenum format = (chan == 4) ? GL_RGBA : GL_RGB;
			
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, c, format, w, h, 0, format, GL_UNSIGNED_BYTE, pxls);

			stbi_image_free(pxls); 
		}
	}

	return texObject;
}

GLuint setup_white_texture()
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	
	unsigned char data[] = { 255, 255, 255 };

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return textureID;
}