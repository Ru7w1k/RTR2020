#pragma once

// headers
#include <vector>

#include "main.h"
#include "vmath.h"

using namespace std;
using namespace vmath;

// types
typedef struct _Sphere 
{
	vec3  center;
	float radius;
	float mass;
	vec3 vel;
	vec3 acc;
} Sphere;

typedef struct _Plane
{
	vec3 normal;
	float distance;
} Plane;

typedef struct _OBB
{
	vec3 position;
	vec3 size;
	mat3 orientation;
};


typedef struct _World
{
	vector<Sphere*> Spheres;
	vector<Plane*> Planes;
	vec3 WorldP1; // top-left point of world space
	vec3 WorldP2; // bottom-right point of world space

} World;

// functions
void addSphere(World&, vec3, vec3, float, float);
void addPlane(World&, vec3, float);
void step(World&, float);
void deleteWorld(World&);

// helper functions
float getRand(float min, float max);
vec3 getRandVec3(float min, float max);
