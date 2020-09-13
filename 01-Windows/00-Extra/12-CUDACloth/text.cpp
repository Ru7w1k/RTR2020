// headers 
#include <Windows.h>
#include <iostream>
#include <map>
#include <string>

#include <GL/glew.h>
#include <gl/GL.h>

#include "vmath.h"

#include <ft2build.h>
#include FT_FREETYPE_H

// libraries
#pragma comment(lib,"freetype.lib")

using namespace std;
using namespace vmath;

extern FILE  *gpFile;

// global data
struct Character
{
	GLuint TextureID; // ID handle of glyph
	ivec2 Size; //  Size of glyph
	ivec2 Bearing; // offset from baseline to left/top of glyph
	GLuint Advance; // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character>Characters;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gTextShaderProgram;
GLuint vao_text;
GLuint vbo_text;

// function prototypes
void InitFont(void);
void RenderText(std::string, mat4, GLfloat, GLfloat, GLfloat, vmath::vec3);

// functions
void RenderText(string text, mat4 mvp, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
	glUseProgram(gTextShaderProgram);
	glUniform3fv(glGetUniformLocation(gTextShaderProgram, "textColor"), 1, color);
	glUniformMatrix4fv(glGetUniformLocation(gTextShaderProgram, "u_mvp_matrix"), 1, GL_FALSE, mvp);
	
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(vao_text);
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing[0] * scale;
		GLfloat ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

		GLfloat w = ch.Size[0] * scale;
		GLfloat h = ch.Size[1] * scale;

		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0},
			{ xpos, ypos,     0.0, 1.0},
			{ xpos + w, ypos, 1.0, 1.0},
			{ xpos, ypos + h, 0.0, 0.0},
			{ xpos + w, ypos, 1.0, 1.0},
			{ xpos + w, ypos + h, 1.0,0.0}
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_text);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale;

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void InitFont(void)
{
    void uninitialize(void);
	BOOL loadTexture(GLuint *texture, TCHAR imageResourceID[]);

	//Vertex shader//
	//create
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *vertexShaderSourceCode =
		"#version 450 core"\
		"\n"\

		"in vec4 vPosition;"\

		"out vec2 TexCoords;"\

		"uniform mat4 u_mvp_matrix;"\

		"void main()"\
		"{"\
		"	gl_Position = u_mvp_matrix * vec4(vPosition.xy, 0.0, 1.0);"\
		"	TexCoords = vPosition.zw;"
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);

	GLint iInfoLength = 0;
	GLint iShaderCompiledStatus = 0;
	char * szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//Fragment shader
	//createShader

	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode =
		"#version 450 core"\
		"\n"\

		"in vec2 TexCoords;"\

		"out vec4 color;"\

		"uniform sampler2D text;"\
		"uniform vec3 textColor;"\

		"void main()"\
		"{"\
			"vec2 uv=TexCoords.xy;"\

			"vec4 sampled = vec4(1.0,1.0,1.0,texture(text, uv).r);"\
			"color = vec4(textColor, 1.0) * sampled;"\
		"}";


	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment shader compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//shader program
	//create

	gTextShaderProgram = glCreateProgram();

	glAttachShader(gTextShaderProgram, gVertexShaderObject);
	glAttachShader(gTextShaderProgram, gFragmentShaderObject);

	glLinkProgram(gTextShaderProgram);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gTextShaderProgram, GL_LINK_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gTextShaderProgram, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gTextShaderProgram, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Status %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		MessageBox(NULL, TEXT("ERROR::FREETYPE: Could Not init FreeType Library"), TEXT("ERROR"), MB_OK);
	FT_Face face;
	if (FT_New_Face(ft, "C:\\Windows\\WinSxS\\amd64_microsoft-windows-font-truetype-consolas_31bf3856ad364e35_10.0.19041.1_none_1fe0609844af8bce\\consola.ttf", 0, &face))
		MessageBox(NULL, TEXT("ERROR::FREETYPE: Failed to load font"), TEXT("ERROR"), MB_OK);
	FT_Set_Pixel_Sizes(face, 0, 175);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			MessageBox(NULL, TEXT("ERROR::FREETYPE: Failed to load Glyph"), TEXT("ERROR"), MB_OK);
			continue;
		}
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			vmath::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			vmath::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(GLuint)face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &vao_text);
	glBindVertexArray(vao_text);

	glGenBuffers(1, &vbo_text);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_text);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

