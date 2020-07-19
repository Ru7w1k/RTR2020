/* trace-prepare shader program */

#pragma once

#include <GL/glew.h>
#include <gl/GL.h>

const GLchar* vsTraceShaderSourceCode = (GLchar*)
	"#version 460 core \n" \
	
	"out VS_OUT \n" \
	"{ \n" \
	"	vec3 ray_origin; \n" \
	"	vec3 ray_direction; \n" \
	"} vs_out; \n" \

	"uniform vec3 ray_origin; \n" \
	"uniform mat4 ray_lookat; \n" \
	"uniform float aspect = 0.75; \n" \

	"uniform vec3 direction_scale = vec3(1.9, 1.9, 1.0); \n" \
	"uniform vec3 direction_bias = vec3(0.0, 0.0, 0.0); \n" \

	"void main(void) \n" \
	"{ \n" \
	"   vec4 vertices[4] = vec4[4]( \n" \
	"       vec4(-1.0, -1.0, 1.0, 1.0), \n" \
	"       vec4( 1.0, -1.0, 1.0, 1.0), \n" \
	"       vec4(-1.0,  1.0, 1.0, 1.0), \n" \
	"       vec4( 1.0,  1.0, 1.0, 1.0) \n" \
	"   ); \n" \

	"   vec4 pos = vertices[gl_VertexID]; \n" \
	"   gl_Position = pos; \n" \
	
	"	vs_out.ray_origin = ray_origin * vec3(1.0, 1.0, -1.0); \n" \
	"	vs_out.ray_direction = (ray_lookat * vec4(pos.xyz * direction_scale * vec3(1.0, aspect, 2.0) + direction_bias, 0.0)).xyz; \n" \
	"} \n";


const GLchar* fsTraceShaderSourceCode = (GLchar*)
	"#version 460 core \n" \

	"in VS_OUT \n" \
	"{ \n" \
	"	vec3 ray_origin; \n" \
	"	vec3 ray_direction; \n" \
	"} fs_in; \n" \

	"layout (location = 0) out vec3 color; \n" \
	"layout (location = 1) out vec3 origin; \n" \
	"layout (location = 2) out vec3 reflected; \n" \
	"layout (location = 3) out vec3 refracted; \n" \
	"layout (location = 4) out vec3 reflected_color; \n" \
	"layout (location = 5) out vec3 refracted_color; \n" \

	"void main(void) \n" \
	"{ \n" \
	"   color = vec3(0.0); \n" \
	"   origin = fs_in.ray_origin; \n" \
	"   reflected = fs_in.ray_direction; \n" \
	"   refracted = vec3(0.0); \n" \
	"   reflected_color = vec3(1.0); \n" \
	"   refracted_color = vec3(0.0); \n" \
	"} \n";

