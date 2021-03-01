// headers
#include "shader.h"
#include "scene.h"
#include "objects.h"
#include "physics.h"

// namespace
using namespace vmath;

// globals
deShader *mainShader = NULL;
GLuint uMMatrix;
GLuint uVMatrix;
GLuint uPMatrix;
GLuint uPBRMaps[5];
GLuint texPBRStone[5];
GLuint texPBRWood[5];

World w;

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
		"uniform sampler2D u_nor;" \

		"uniform float f;" \

		"out VS_OUT {" \
		"	vec4 worldPos;" \
		"	vec3 normal;" \
		"	vec2 tc;" \
		"   vec3 viewer_vector; " \
		"} vs_out;" \

		"void main (void)" \
		"{" \
		"   vec4 disp = (2.0 * texture(u_disp, vTexcoord)) - vec4(1.0);" \
		"   vec4 nor = (2.0 * texture(u_nor, vTexcoord)) - vec4(1.0);" \
		"   vec4 vPos = vPosition;" \
		"   vPos += vec4(nor.xyz * disp.r, 1.0);" \

		"   vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPos;" \
		"   vs_out.viewer_vector = vec3(-eye_coordinates.xyz);" \

		"	vs_out.normal = vNormal;" \
		"	vs_out.tc = vTexcoord*3.0;" \
		"	vs_out.worldPos = u_m_matrix * vPos;" \
		"	gl_Position = u_p_matrix * u_v_matrix * vs_out.worldPos;" \
		"}";


	// fragment shader source code
	const GLchar* fragmentShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in VS_OUT {" \
		"	vec4 worldPos;" \
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

		"uniform vec4 light_direction = vec4(0.0, 10.0, 2.0, 1.0);" \

		"out vec4 FragColor;" \

		"void main (void)" \
		"{" \
		"   vec4 nor = (2.0 * texture(u_nor, fs_in.tc)) - vec4(1.0);" \

		"	vec3 Q1  = dFdx(fs_in.worldPos.xyz);" \
		"	vec3 Q2  = dFdy(fs_in.worldPos.xyz);" \
		"	vec2 st1 = dFdx(fs_in.tc);" \
		"	vec2 st2 = dFdy(fs_in.tc);" \

		"   vec3 tnorm = mat3(u_m_matrix) * fs_in.normal; " \
		"	vec3 N = normalize(tnorm);" \
		"	vec3 T = normalize(Q1*st2.t - Q2*st1.t);" \
		"	vec3 B = -normalize(cross(N, T));" \
		"	mat3 TBN = mat3(T, B, N);" \
		"   vec3 ntnorm = normalize(TBN * nor.xyz);" \

		"   vec3 nlight_direction = normalize(light_direction.xyz);" \
		"   vec3 nviewer_vector = normalize(fs_in.viewer_vector);" \
		"   vec3 reflection_vector = reflect(-nlight_direction, ntnorm);" \
		"   float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);" \

		"   vec3 diffuse  = texture(u_diff, fs_in.tc).rgb * texture(u_ao, fs_in.tc).rgb * tn_dot_ldir;" \
		"   vec3 specular = vec3(0.1) * pow(max(dot(reflection_vector, nviewer_vector), 0.0), 128.0*texture(u_rough, fs_in.tc).r);" \
		"   vec3 ambient = vec3(0.1) * texture(u_ao, fs_in.tc).rgb;" \
		"   vec3 phong_ads_light = diffuse + specular + ambient;" \

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

	// textures
	glEnable(GL_TEXTURE_2D);
	loadTexture(&texPBRStone[0], MAKEINTRESOURCE(ROCKS_DISP));
	loadTexture(&texPBRStone[1], MAKEINTRESOURCE(ROCKS_AO));
	loadTexture(&texPBRStone[2], MAKEINTRESOURCE(ROCKS_DIFF));
	loadTexture(&texPBRStone[3], MAKEINTRESOURCE(ROCKS_NOR));
	loadTexture(&texPBRStone[4], MAKEINTRESOURCE(ROCKS_ROUGH));

	loadTexture(&texPBRWood[0], MAKEINTRESOURCE(WOOD_DISP));
	loadTexture(&texPBRWood[1], MAKEINTRESOURCE(WOOD_AO));
	loadTexture(&texPBRWood[2], MAKEINTRESOURCE(WOOD_DIFF));
	loadTexture(&texPBRWood[3], MAKEINTRESOURCE(WOOD_NOR));
	loadTexture(&texPBRWood[4], MAKEINTRESOURCE(WOOD_ROUGH));
	glDisable(GL_TEXTURE_2D);

	// physics objects
	for(int i = 0; i < 10; i++)
		addSphere(w, 
			vec3(getRand(-10.0f, 10.0f), getRand(0.0f, 10.0f), getRand(-10.0f, 10.0f)), 
			getRandVec3(-50.0f, 50.0f),
			getRand(0.2f, 1.2f), 1.0f);

	/*addSphere(w, vec3(-2.0f, 0.5f, 3.0f), 1.0f, 1.0f);
	addSphere(w, vec3(0.0f, 1.0f,  0.0f), 0.7f, 1.0f);
	addSphere(w, vec3(2.0f, 1.0f, -3.0f), 0.5f, 1.0f);
	addSphere(w, vec3(4.1f, 6.0f, -3.0f), 1.0f, 1.0f);
	addSphere(w, vec3(4.0f, 2.0f, -3.0f), 1.2f, 1.0f);

	addSphere(w, vec3(0.1f, 3.0f, 0.0f), 0.7f, 1.0f);
	addSphere(w, vec3(-2.0f, 2.2f, 3.4f), 1.0f, 1.0f);
	addSphere(w, vec3(2.2f, 4.0f, -3.0f), 0.5f, 1.0f);
	addSphere(w, vec3(4.1f, 7.4f, -3.7f), 1.0f, 1.0f);
	addSphere(w, vec3(4.7f, 4.0f, -1.0f), 1.2f, 1.0f);*/

	return true;
}

void DrawScene(void)
{
	// extern variables
	extern mat4 matProjection;

	// variables 
	static float angle = 0.0f;
	mat4 matModel = mat4::identity();
	mat4 matView = mat4::identity();

	// code
	matView = lookat(
		vec3(25.0f*cosf(angle), 10.0f, 25.0f*sinf(angle)),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	);

	// step through time in world
	//for (int i = 0; i < 100; i++)
	step(w, 0.01f);

	glEnable(GL_TEXTURE_2D);
	glUseProgram(mainShader->shaderProgram);
	glUniformMatrix4fv(uVMatrix, 1, false, matView);
	glUniformMatrix4fv(uPMatrix, 1, false, matProjection);

	for (int i = 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texPBRWood[i]);
		glUniform1i(uPBRMaps[i], i);
	}

	for (int i = 0; i < w.Spheres.size(); i++)
	{
		matModel = translate(w.Spheres[i]->center);
		matModel *= scale(w.Spheres[i]->radius);
		glUniformMatrix4fv(uMMatrix, 1, false, matModel);
		DrawSphere();
	}

	// ground level
	for (int i = 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texPBRStone[i]);
		glUniform1i(uPBRMaps[i], i);
	}
	
	matModel = translate(0.0f, -2.0f, 0.0f);
	matModel *= scale(25.0f, 1.0f, 25.0f);
	glUniformMatrix4fv(uMMatrix, 1, false, matModel);
	DrawQuad();

	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);

	// update state
	angle += 0.001f;
}

void UnInitScene(void)
{
	deleteWorld(w);
	glDeleteTextures(5, texPBRStone);
	deShaderUninit(mainShader);
}

void KbdHandleScene(WPARAM wParam, LPARAM lParam)
{
	addSphere(w,
		vec3(getRand(-10.0f, 10.0f), getRand(0.0f, 10.0f), getRand(-10.0f, 10.0f)),
		getRandVec3(-50.0f, 50.0f),
		getRand(0.2f, 2.2f), 1.0f);
}

