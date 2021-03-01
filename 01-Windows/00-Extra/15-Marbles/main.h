#pragma once

// headers
#include <windows.h>
#include <ShellScalingApi.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include "vmath.h"

#define deLog(...)   fprintf(gpFile, __VA_ARGS__);fprintf(gpFile, "\n");

// enums
enum 
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

// globals
extern "C" FILE* gpFile;

// resources
#define RMC_ICON		101

#define ROCKS_DISP		200
#define ROCKS_AO		201
#define ROCKS_DIFF		202
#define ROCKS_NOR		203
#define ROCKS_ROUGH		204

#define WOOD_DISP		210
#define WOOD_AO			211
#define WOOD_DIFF		212
#define WOOD_NOR		213
#define WOOD_ROUGH		214



