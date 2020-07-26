/* blit shader program */
#include <GL/gl.h>

#pragma once

const GLchar *vsBlitShaderSourceCode = (GLchar *)
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


const GLchar *fsBlitShaderSourceCode = (GLchar *) 
    "#version 460 core \n" \

    "layout (binding = 0) uniform sampler2D tex_composite; \n" \

    "layout (location = 0) out vec4 color; \n" \

    "void main(void) \n" \
    "{ \n" \
    "   color = texelFetch(tex_composite, ivec2(gl_FragCoord.xy), 0); \n" \
    "} \n";





