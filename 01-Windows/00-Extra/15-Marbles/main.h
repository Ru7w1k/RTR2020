#pragma once

// headers
#include <windows.h>
#include <ShellScalingApi.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include "vmath.h"

#define deLog(...)   fprintf(gpFile, __VA_ARGS__);fprintf(gpFile, "\n")

// enums
enum 
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

// globals
extern FILE* gpFile;

// resources
#define RMC_ICON 101
