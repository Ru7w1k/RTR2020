// headers
#include "shader.h"
#include "scene.h"

// namespace
using namespace vmath;

// globals
deShader *mainShader = NULL;
GLuint vao;
GLuint vbo;

// code
bool InitScene(void)
{
	// vertex shader source code 
	const GLchar* vertexShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \
		"layout (location = 0) in vec4 vPosition;" \
		"layout (location = 1) in vec4 vColor;" \
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

	// vertex array
	const GLfloat cubeData[] = {
		/* Top */
		 1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,

		/* Bottom */
		 1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,

		/* Front */
		 1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,

		/* Back */
		 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,

		/* Right */
		1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,

		/* Left */
		-1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f
	};

	// create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex position
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	// vertex colors
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	// vertex normals
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	// vertex texcoords
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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
	matModel = translate(0.0f, 0.0f, -6.0f);
	matModel *= rotate(angle, angle, angle);

	glUseProgram(mainShader->shaderProgram);
	glBindVertexArray(vao);

	glUniformMatrix4fv(glGetUniformLocation(mainShader->shaderProgram, "u_mvp_matrix"), 1, false, matProjection * matModel);

	// draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	
	glBindVertexArray(0);
	glUseProgram(0);

	// update state
	angle += 1.0f;
}

void UnInitScene(void)
{
	deShaderUninit(mainShader);
}

