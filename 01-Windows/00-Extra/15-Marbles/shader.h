#pragma once

#include "main.h"

// heShader: defines a shader object
typedef struct _deShader {

	GLuint vertexShader;
	GLuint tessControlShader;
	GLuint tessEvaluationShader;
	GLuint geometryShader;
	GLuint fragmentShader;
	GLuint shaderProgram;

} deShader;

int deCreateShader(GLuint* shader, GLenum type, const char* code)
{
	// create shader
	*shader = glCreateShader(type);

	// attach source code to shader
	glShaderSource(*shader, 1, (const GLchar**)&code, NULL);

	// compile vertex shader source code
	glCompileShader(*shader);

	// compilation errors 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(*shader, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				deLog("Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
			}
		}
	}

	return iShaderCompileStatus;
}

int deCreateProgram(deShader* shader)
{
	// create shader program object 
	shader->shaderProgram = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(shader->shaderProgram, shader->vertexShader);

	// attach fragment shader to shader program
	glAttachShader(shader->shaderProgram, shader->fragmentShader);

	// link the shader program
	glLinkProgram(shader->shaderProgram);

	// linking errors
	GLint iProgramLinkStatus = 0;
	GLint iInfoLogLength = 0;
	char* szInfoLog = NULL;

	glGetProgramiv(shader->shaderProgram, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shader->shaderProgram, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shader->shaderProgram, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				deLog("Shader Program Linking Info Log: %s", szInfoLog);
				free(szInfoLog);
			}
		}
	}

	return iProgramLinkStatus;
}

// create a new shader program object using vertex and fragment shader source code
deShader* deNewShader(const char* vertexShaderCode, const char* fragmentShaderCode,
	const char* tessellationControlCode = NULL, const char* tessellationEvaluationCode = NULL,
	const char* geometryShaderCode = NULL)
{
	int compileStatus;
	int linkStatus;
	deShader* shader = (deShader*)malloc(sizeof(deShader));
	compileStatus = deCreateShader(&(shader->vertexShader), GL_VERTEX_SHADER, vertexShaderCode);
	if (compileStatus == GL_FALSE)
	{
		deLog("Vertex shader compilation failed...");
		return NULL;
	}

	if (NULL != tessellationControlCode)
	{
		compileStatus = deCreateShader(&(shader->tessControlShader), GL_TESS_CONTROL_SHADER, tessellationControlCode);
		if (compileStatus == GL_FALSE)
		{
			deLog("Tessellation control shader compilation failed...");
			return NULL;
		}
	}

	if (NULL != tessellationEvaluationCode)
	{
		compileStatus = deCreateShader(&(shader->tessEvaluationShader), GL_TESS_CONTROL_SHADER, tessellationEvaluationCode);
		if (compileStatus == GL_FALSE)
		{
			deLog("Tessellation evaluation shader compilation failed...");
			return NULL;
		}
	}

	if (NULL != geometryShaderCode)
	{
		compileStatus = deCreateShader(&(shader->geometryShader), GL_GEOMETRY_SHADER, geometryShaderCode);
		if (compileStatus == GL_FALSE)
		{
			deLog("Geometry shader compilation failed...");
			return NULL;
		}
	}

	compileStatus = deCreateShader(&(shader->fragmentShader), GL_FRAGMENT_SHADER, fragmentShaderCode);
	if (compileStatus == GL_FALSE)
	{
		deLog("Fragment shader compilation failed...");
		return NULL;
	}

	linkStatus = deCreateProgram(shader);
	if (compileStatus == GL_FALSE)
	{
		deLog("Program linking failed...");
		return NULL;
	}

	return shader;
}

// delete shader program object
void deShaderUninit(deShader* shader)
{
	if (shader == NULL) { return; }

	GLsizei shaderCount;
	GLsizei shaderNumber;

	glUseProgram(shader->shaderProgram);
	glGetProgramiv(shader->shaderProgram, GL_ATTACHED_SHADERS, &shaderCount);

	GLuint* pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
	if (pShaders)
	{
		glGetAttachedShaders(shader->shaderProgram, shaderCount, &shaderCount, pShaders);

		for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
		{
			// detach shader
			glDetachShader(shader->shaderProgram, pShaders[shaderNumber]);

			// delete shader
			glDeleteShader(pShaders[shaderNumber]);
			pShaders[shaderNumber] = 0;
		}
		free(pShaders);
	}

	glDeleteProgram(shader->shaderProgram);
	shader->shaderProgram = 0;
	glUseProgram(0);
}

