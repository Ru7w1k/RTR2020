/* raytracer shader program */
#include <GL/gl.h>

#pragma once

const GLchar *vsRaytracerSourceCode = (GLchar *)
    "#version 460 core \n" \

    "void main(void) \n" \
    "{ \n" \
    "   vec4 vertices[4] = vec4[4]( \n" \
    "       vec4(-1.0, -1.0, 0.5, 1.0) \n" \
    "       vec4( 1.0, -1.0, 0.5, 1.0) \n" \
    "       vec4(-1.0,  1.0, 0.5, 1.0) \n" \
    "       vec4( 1.0,  1.0, 0.5, 1.0) \n" \
    "   ); \n" \

    "   gl_Position = vertices[gl_VertexID]; \n" \

    "} \n";


const GLchar *fsRaytracerSourceCode = (GLchar *) 
    "#version 460 core \n" \

    "layout (location = 0) out vec3 color; \n" \
    "layout (location = 1) out vec3 position; \n" \
    "layout (location = 2) out vec3 reflected; \n" \
    "layout (location = 3) out vec3 refracted; \n" \
    "layout (location = 4) out vec3 reflected_color; \n" \
    "layout (location = 5) out vec3 refracted_color; \n" \

    "layout (binding = 0) uniform sampler2D tex_origin; \n" \
    "layout (binding = 1) uniform sampler2D tex_direction; \n" \
    "layout (binding = 2) uniform sampler2D tex_color; \n" \

    "struct ray \n" \
    "{ \n" \
    "   vec3 origin; \n" \
    "   vec3 direction; \n" \
    "}; \n" \

    "struct sphere \n" \
    "{ \n" \
    "   vec3 center; \n" \
    "   float radius; \n" \
    "   vec4 color; \n" \
    "}; \n" \

    "struct light \n" \
    "{ \n" \
    "   vec3 position; \n" \
    "}; \n" \

    "layout (std140, binding = 1) uniform SPHERES \n" \
    "{ \n" \
    "   sphere S[128]; \n" \
    "}; \n" \

    "layout (std140, binding = 2) uniform PLANES \n" \
    "{ \n" \
    "   vec4 P[128]; \n" \
    "}; \n" \

    "layout (std140, binding = 3) uniform LIGHTS \n" \
    "{ \n" \
    "   light L[120]; \n" \
    "} lights; \n" \
    

    

    "void main(void) \n" \
    "{ \n" \
    "   color = texelFetch(tex_composite, ivec2(gl_FragCoord.xy), 0); \n" \
    "} \n";





