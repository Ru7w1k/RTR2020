// headers
#include "shader.h"
#include "scene.h"
#include "objects.h"

// namespace
using namespace vmath;

// globals
deShader *mainShader = NULL;
GLuint uMMatrix;
GLuint uVMatrix;
GLuint uPMatrix;
GLuint uPBRMaps[5];
GLuint texPBRMaps[5];

// code
bool InitScene(void)
{
	// vertex shader source code 
	const GLchar* vertexShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"in vec2 vTexcoord;" \

		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \

		"uniform sampler2D u_disp;" \

		"uniform float f;" \

		"out VS_OUT {" \
		"	vec3 normal;" \
		"	vec2 tc;" \
		"   vec3 viewer_vector; " \
		"} vs_out;" \

		"void main (void)" \
		"{" \
		"   vec4 disp = (2.0 * texture(u_disp, vTexcoord)) - vec4(1.0);" \
		"   vec4 vPos = vPosition;" \
		"   vPos.y += disp.r*1.0f;" \

		"   vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPos;" \
		"   vs_out.viewer_vector = vec3(-eye_coordinates.xyz);" \

		"	vs_out.normal = vNormal;" \
		"	vs_out.tc = vTexcoord;" \
		"	gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPos;" \
		"}";


	// fragment shader source code
	const GLchar* fragmentShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in VS_OUT {" \
		"	vec3 normal;" \
		"	vec2 tc;" \
		"   vec3 viewer_vector; " \
		"} fs_in;" \

		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \

		"uniform sampler2D u_ao;" \
		"uniform sampler2D u_diff;" \
		"uniform sampler2D u_nor;" \
		"uniform sampler2D u_rough;" \

		"uniform vec4 light_direction = vec4(4.0, 10.0, 4.0, 1.0);" \

		"out vec4 FragColor;" \

		"void main (void)" \
		"{" \
		"   vec4 nor = (2.0 * texture(u_nor, fs_in.tc)) - vec4(1.0);" \
		"   nor = vec4(fs_in.normal, 0.0);" \
		"   vec3 tnorm = mat3(u_v_matrix * u_m_matrix) * nor.xyz; " \

		"   vec3 ntnorm = normalize(tnorm);" \
		"   vec3 nlight_direction = normalize(light_direction.xyz);" \
		"   vec3 nviewer_vector = normalize(fs_in.viewer_vector);" \
		"   vec3 reflection_vector = reflect(-nlight_direction, ntnorm);" \
		"   float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);" \

		"   vec3 diffuse  = texture(u_diff, fs_in.tc).rgb * texture(u_ao, fs_in.tc).rgb * tn_dot_ldir;" \
		"   vec3 specular = vec3(0.1) * pow(max(dot(reflection_vector, nviewer_vector), 0.0), 128.0*texture(u_rough, fs_in.tc).r);" \
		"   vec3 phong_ads_light = diffuse + specular;" \

		"	FragColor = vec4(phong_ads_light, 1.0);" \
		"}";

	mainShader = deNewShader(vertexShaderSourceCode, fragmentShaderSourceCode);
	if (mainShader == NULL)
	{
		deLog("mainShader failed...");
		return false;
	}

	// uniforms
	uMMatrix = glGetUniformLocation(mainShader->shaderProgram, "u_m_matrix");
	uVMatrix = glGetUniformLocation(mainShader->shaderProgram, "u_v_matrix");
	uPMatrix = glGetUniformLocation(mainShader->shaderProgram, "u_p_matrix");

	uPBRMaps[0] = glGetUniformLocation(mainShader->shaderProgram, "u_disp");
	uPBRMaps[1] = glGetUniformLocation(mainShader->shaderProgram, "u_ao");
	uPBRMaps[2] = glGetUniformLocation(mainShader->shaderProgram, "u_diff");
	uPBRMaps[3] = glGetUniformLocation(mainShader->shaderProgram, "u_nor");
	uPBRMaps[4] = glGetUniformLocation(mainShader->shaderProgram, "u_rough");

	glEnable(GL_TEXTURE_2D);
	loadTexture(&texPBRMaps[0], MAKEINTRESOURCE(ROCKS_DISP));
	loadTexture(&texPBRMaps[1], MAKEINTRESOURCE(ROCKS_AO));
	loadTexture(&texPBRMaps[2], MAKEINTRESOURCE(ROCKS_DIFF));
	loadTexture(&texPBRMaps[3], MAKEINTRESOURCE(ROCKS_NOR));
	loadTexture(&texPBRMaps[4], MAKEINTRESOURCE(ROCKS_ROUGH));
	glDisable(GL_TEXTURE_2D);

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
	glEnable(GL_TEXTURE_2D);
	glUseProgram(mainShader->shaderProgram);
	glUniformMatrix4fv(uVMatrix, 1, false, mat4::identity());
	glUniformMatrix4fv(uPMatrix, 1, false, matProjection);

	for (int i = 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texPBRMaps[i]);
		glUniform1i(uPBRMaps[i], 0);
	}

	matModel = translate(2.0f, 0.0f, -6.0f);
	matModel *= rotate(angle, angle, angle);
	glUniformMatrix4fv(uMMatrix, 1, false, matModel);
	DrawCube();

	matModel = translate(-2.0f, 0.0f, -6.0f);
	matModel *= rotate(angle, angle, angle);
	glUniformMatrix4fv(uMMatrix, 1, false, matModel);
	DrawSphere();
	
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);

	// update state
	angle += 1.0f;
}

void UnInitScene(void)
{
	glDeleteTextures(5, texPBRMaps);
	deShaderUninit(mainShader);
}

