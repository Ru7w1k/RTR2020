// headers
#include "shader.h"
#include "scene.h"
#include "objects.h"

// namespace
using namespace vmath;

// globals
deShader *mainShader = NULL;
GLuint uMVPMatrix;

// code
bool InitScene(void)
{
	// vertex shader source code 
	const GLchar* vertexShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_Color;" \
		"void main (void)" \
		"{" \
		"	gl_Position = u_mvp_matrix * vPosition;" \
		"	out_Color = vColor;" \
		"}";


	// fragment shader source code
	const GLchar* fragmentShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \
		"in vec4 out_Color;" \
		"out vec4 FragColor;" \
		"void main (void)" \
		"{" \
		"	FragColor = out_Color;" \
		"}";

	mainShader = deNewShader(vertexShaderSourceCode, fragmentShaderSourceCode);
	if (mainShader == NULL)
	{
		deLog("mainShader failed...");
		return false;
	}

	// uniforms
	uMVPMatrix = glGetUniformLocation(mainShader->shaderProgram, "u_mvp_matrix");

	return true;
}

void DrawScene(void)
{
	// extern variables
	extern mat4 matProjection;

	// variables 
	static float angle = 0.0f;
	mat4 matModel = mat4::identity();

	// code
	glUseProgram(mainShader->shaderProgram);

	matModel = translate(2.0f, 0.0f, -6.0f);
	matModel *= rotate(angle, angle, angle);
	glUniformMatrix4fv(uMVPMatrix, 1, false, matProjection * matModel);
	DrawCube();

	matModel = translate(-2.0f, 0.0f, -6.0f);
	matModel *= rotate(angle, angle, angle);
	glUniformMatrix4fv(uMVPMatrix, 1, false, matProjection * matModel);
	DrawSphere();
	
	glUseProgram(0);

	// update state
	angle += 1.0f;
}

void UnInitScene(void)
{
	deShaderUninit(mainShader);
}

