// headers
#include "physics.h"

const float t = 0.000005f;

// functions
void addSphere(World& w, vec3 c, vec3 u, float r, float m)
{
	Sphere* s = (Sphere*)malloc(sizeof(Sphere));
	ZeroMemory(s, sizeof(Sphere));
	s->center = c;
	s->radius = r;
	s->mass = m;
	s->vel = u;
	w.Spheres.push_back(s);
}

void addPlane(World& w, vec3 n, float d)
{
	Plane* p = (Plane*)malloc(sizeof(Plane));
	ZeroMemory(p, sizeof(Plane));
	p->normal = n;
	p->distance = d;
	w.Planes.push_back(p);
}

void step(World& w, float t)
{
	for (int i = 0; i < w.Spheres.size(); i++)
	{
		vec3 F = vec3(0.0f, -9.8f, 0.0f);
		vec3 a = F / w.Spheres[i]->mass;
		vec3 s = w.Spheres[i]->vel * t + 0.5f * a * t * t;
		vec3 v = w.Spheres[i]->vel + a * t;

		w.Spheres[i]->vel = v;
		w.Spheres[i]->center = w.Spheres[i]->center + s;


		// resolve collisions
		for (int j = i+1; j < w.Spheres.size(); j++)
		{
			float radiusDist = w.Spheres[i]->radius + w.Spheres[j]->radius;
			vec3 direction = w.Spheres[j]->center - w.Spheres[i]->center;
			float centerDist = length(direction);
			direction /= centerDist;

			float dist = centerDist - radiusDist;

			if (dist < 0)
			{
				vec3 dir = normalize(direction * dist);
				vec3 otherDir = reflect(dir, normalize(w.Spheres[i]->vel));

				w.Spheres[i]->vel = 0.80f * reflect(w.Spheres[i]->vel, otherDir);
				w.Spheres[j]->vel = 0.80f * reflect(w.Spheres[j]->vel, dir);

				w.Spheres[i]->center += dist / 2.0f * otherDir;
				w.Spheres[j]->center += dist / 2.0f * dir;

			}
		}

		for (int k = 0; k < w.Planes.size(); k++)
		{

		}



		// ground plane at y = -2
		float dist = distance(w.Spheres[i]->center, vec3(w.Spheres[i]->center[0], -2.0f, w.Spheres[i]->center[2]));
		if (dist < w.Spheres[i]->radius)
		{
			w.Spheres[i]->center[1] += (w.Spheres[i]->radius - dist);
			w.Spheres[i]->vel = 0.80f * reflect(w.Spheres[i]->vel, vec3(0.0f, 1.0f,0.0f));
		}

		// ground plane at y = 2
		dist = distance(w.Spheres[i]->center, vec3(w.Spheres[i]->center[0], 2.0f, w.Spheres[i]->center[2]));
		if (dist < w.Spheres[i]->radius)
		{
			w.Spheres[i]->center[1] += (w.Spheres[i]->radius - dist);
			w.Spheres[i]->vel = 0.80f * reflect(w.Spheres[i]->vel, vec3(0.0f, -1.0f, 0.0f));
		}

		// ground plane at z = -25
		dist = distance(w.Spheres[i]->center, vec3(w.Spheres[i]->center[0], w.Spheres[i]->center[1], -25.0f));
		if (dist < w.Spheres[i]->radius)
		{
			w.Spheres[i]->center[2] += (w.Spheres[i]->radius - dist);
			w.Spheres[i]->vel = 0.80f * reflect(w.Spheres[i]->vel, vec3(0.0f, 0.0f, 1.0f));
		}

		// ground plane at z = 25
		dist = distance(w.Spheres[i]->center, vec3(w.Spheres[i]->center[0], w.Spheres[i]->center[1], 25.0f));
		if (dist < w.Spheres[i]->radius)
		{
			w.Spheres[i]->center[2] -= (w.Spheres[i]->radius - dist);
			w.Spheres[i]->vel = 0.80f * reflect(w.Spheres[i]->vel, vec3(0.0f, 0.0f, -1.0f));
		}

		// ground plane at x = -25
		dist = distance(w.Spheres[i]->center, vec3(-25.0f, w.Spheres[i]->center[1], w.Spheres[i]->center[2]));
		if (dist < w.Spheres[i]->radius)
		{
			w.Spheres[i]->center[0] += (w.Spheres[i]->radius - dist);
			w.Spheres[i]->vel = 0.80f * reflect(w.Spheres[i]->vel, vec3(1.0f, 0.0f, 0.0f));
		}

		// ground plane at x = 25
		dist = distance(w.Spheres[i]->center, vec3(25.0f, w.Spheres[i]->center[1], w.Spheres[i]->center[2]));
		if (dist < w.Spheres[i]->radius)
		{
			w.Spheres[i]->center[0] -= (w.Spheres[i]->radius - dist);
			w.Spheres[i]->vel = 0.80f * reflect(w.Spheres[i]->vel, vec3(-1.0f, 0.0f, 0.0f));
		}

		if (w.Spheres[i]->center[0] > 25.0f || w.Spheres[i]->center[0] < -25.0f ||
			w.Spheres[i]->center[2] > 25.0f || w.Spheres[i]->center[2] < -25.0f ||
			w.Spheres[i]->center[1] < -2.0f)
		{
			//DebugBreak();
		}
	}
}


void addPlane(World&);

void deleteWorld(World& w)
{
	for (int i = 0; i < w.Spheres.size(); i++)
	{
		if (w.Spheres[0])
		{
			free(w.Spheres[0]);
			w.Spheres[0] = NULL;
		}
	}
}

// helper functions
float getRand(float min, float max)
{
	return min + ((max - min) * (float)rand() / (float)RAND_MAX);
}

vec3 getRandVec3(float min, float max)
{
	return vec3(getRand(min, max), getRand(min, max), getRand(min, max));
}

