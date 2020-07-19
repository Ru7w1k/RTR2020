/* raytracer shader program */
#pragma once

#include <GL/glew.h>
#include <gl/GL.h>

const GLchar *vsRaytracerSourceCode = (GLchar *)
    "#version 460 core \n" \

    "void main(void) \n" \
    "{ \n" \
    "   vec4 vertices[4] = vec4[4]( \n" \
    "       vec4(-1.0, -1.0, 0.5, 1.0), \n" \
    "       vec4( 1.0, -1.0, 0.5, 1.0), \n" \
    "       vec4(-1.0,  1.0, 0.5, 1.0), \n" \
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
    
    "uniform int num_spheres = 7; \n" \
    "uniform int num_planes  = 6; \n" \
    "uniform int num_lights  = 5; \n" \

    "float intersect_ray_sphere(ray R, sphere S, out vec3 hitpos, out vec3 normal) \n" \
    "{ \n" \
    "   vec3 v = R.origin - S.center; \n" \
    "   float B = 2.0 * dot(R.direction, v); \n" \
    "   float C = dot(v, v) - S.radius * S.radius; \n" \
    "   float B2 = B * B; \n" \
    
    "   float f = B2 - 4.0 * C; \n" \
    
    "   if (f < 0.0) \n" \
    "       return 0.0; \n" \
    
    "   f = sqrt(f); \n" \
    "   float t0 = -B + f; \n" \
    "   float t1 = -B - f; \n" \
    "   float t  = min(max(t0, 0.0), max(t1, 0.0)) * 0.5; \n" \

    "   if (t == 0.0) \n" \
    "       return 0.0; \n" \

    "   hitpos = R.origin + t * R.direction; \n" \
    "   normal = normalize(hitpos - S.center); \n" \

    "   return t; \n" \
    "} \n" \

    "float intersect_ray_plane(ray R, vec4 P, out vec3 hitpos, out vec3 normal) \n" \
    "{ \n" \
    "   vec3 O = R.origin; \n" \
    "   vec3 D = R.direction; \n" \
    "   vec3 N = P.xyz; \n" \
    "   float d = P.w; \n" \

    "   float denom = dot(D, N); \n" \

    "   if (denom == 0.0) \n" \
    "       return 0.0; \n" \

    "   float t  = -(d + dot(O, N)) / denom; \n" \

    "   if (t < 0.0) \n" \
    "       return 0.0; \n" \

    "   hitpos = O + t * D; \n" \
    "   normal = N; \n" \

    "   return t; \n" \
    "} \n" \

    "bool point_visible_to_light(vec3 point, vec3 L) \n" \
    "{ \n" \
    "   return true; \n" \
    "} \n" \

    "vec3 light_point(vec3 position, vec3 normal, vec3 V, light l) \n" \
    "{ \n" \
    "   vec3 ambient = vec3(0.0); \n" \
    
    "   if (!point_visible_to_light(position, l.position)) \n" \
    "       return ambient; \n" \
    "   else \n" \
    "   { \n" \
    "       vec3 L = normalize(l.position - position); \n" \
    "       vec3 N = normal; \n" \
    "       vec3 R = reflect(-L, N); \n" \
    
    "       float rim = clamp(dot(N, V), 0.0, 1.0); \n" \
    "       rim = smoothstep(0.0, 1.0, 1.0 - rim); \n" \
    "       float diff = clamp(dot(N, L), 0.0, 1.0); \n" \
    "       float spec = pow(clamp(dot(R, N), 0.0, 1.0), 260.0); \n" \

    "       vec3 rim_color = vec3(0.0); \n" \
    "       vec3 diff_color = vec3(0.125); \n" \
    "       vec3 spec_color = vec3(0.1); \n" \

    "       return ambient + rim_color * rim + diff_color * diff + spec_color * spec; \n" \

    "   } \n" \
    "} \n" \

    "void main(void) \n" \
    "{ \n" \
    "	ray R; \n" \

    "	R.origin = texelFetch(tex_origin, ivec2(gl_FragCoord.xy), 0).xyz; \n" \
    "	R.direction = normalize(texelFetch(tex_direction, ivec2(gl_FragCoord.xy), 0).xyz); \n" \
    "	vec3 input_color = texelFetch(tex_color, ivec2(gl_FragCoord.xy), 0).rgb; \n" \

    "	vec3 hit_position = vec3(0.0); \n" \
    "	vec3 hit_normal = vec3(0.0); \n" \

    "	color = vec3(0.0); \n" \
    "	position = vec3(0.0); \n" \
    "	reflected = vec3(0.0); \n" \
    "	refracted = vec3(0.0); \n" \
    "	reflected_color = vec3(0.0); \n" \
    "	refracted_color = vec3(0.0); \n" \

    "	if (all(lessThan(input_color, vec3(0.05)))) \n" \
    "	{ \n" \
    "		return; \n" \
    "	} \n" \

    "	R.origin += R.direction * 0.01; \n" \

    "	ray refl; \n" \
    "	ray refr; \n" \
    "	vec3 hitpos; \n" \
    "	vec3 normal; \n" \
    "	float min_t = 1000000.0f; \n" \
    "	int i; \n" \
    "	int sphere_index = 0; \n" \
    "	float t; \n" \

    "	for (i = 0; i < num_spheres; i++) \n" \
    "	{ \n" \
    "		t = intersect_ray_sphere(R, S[i], hitpos, normal); \n" \
    "		if (t != 0.0) \n" \
    "		{ \n" \
    "			if (t < min_t) \n" \
    "			{ \n" \
    "				min_t = t; \n" \
    "				hit_position = hitpos; \n" \
    "				hit_normal = normal; \n" \
    "				sphere_index = i; \n" \
    "			} \n" \
    "		} \n" \
    "	} \n" \

    "	int foobar[] = { 1, 1, 1, 1, 1, 1, 1 }; \n" \

    "	for (i = 0; i < 6; i++) \n" \
    "	{ \n" \
    "		t = intersect_ray_plane(R, P[i], hitpos, normal); \n" \
    "		if (foobar[i] != 0 && t != 0.0) \n" \
    "		{ \n" \
    "			if (t < min_t) \n" \
    "			{ \n" \
    "				min_t = t; \n" \
    "				hit_position = hitpos; \n" \
    "				hit_normal = normal; \n" \
    "				sphere_index = i * 25; \n" \
    "			} \n" \
    "		} \n" \
    "	} \n" \

    "	if (min_t < 100000.0) \n" \
    "	{ \n" \
    "		vec3 my_color = vec3(0.0); \n" \

    "		for (i = 0; i < num_lights; i++) \n" \
    "		{ \n" \
    "			my_color += light_point(hit_position, hit_normal, -R.direction, lights.L[i]); \n" \
    "		} \n" \

    "		my_color *= S[sphere_index].color.rgb; \n" \
    "		color = input_color * my_color; \n" \
    "		vec3 v = normalize(hit_position - R.origin); \n" \
    "		position = hit_position; \n" \
    "		reflected = reflect(v, hit_normal); \n" \
    "		reflected_color = S[sphere_index].color.rgb * 0.5; \n" \
    "		refracted = refract(v, hit_normal, 1.73); \n" \
    "		refracted_color = input_color * S[sphere_index].color.rgb * 0.5; \n" \
    "	} \n" \

    "} \n";




