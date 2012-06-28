#pragma once
#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

typedef struct __vec3 {
	double x;
	double y;
	double z;
} vec3;

vec3 v3_new(double, double, double);
vec3 v3_from_angle(double, double, double);
vec3 v3_unit(double, double);
double v3_length(vec3);
vec3 v3_normalize(vec3);
vec3 v3_negate(vec3);
vec3 v3_add(vec3, vec3);
vec3 v3_sub(vec3, vec3);
double v3_dot(vec3, vec3);

typedef struct __vec3_field {
	int width;
	int height;
	int depth;
	vec3 *data;
} vec3_field;

vec3_field *v3f_new(int, int, int);
vec3_field *v3f_random_unit(int, int, int);
void v3f_destroy(vec3_field *destroy);
vec3 v3f_get(vec3_field *, int, int, int);
void v3f_set(vec3_field *, int, int, int, vec3);

typedef struct __heightmap {
	int width;
	int height;
	double *data;
} heightmap;

heightmap *hm_new(int, int);
void hm_destroy(heightmap *);
double hm_get(heightmap *, int, int);
void hm_set(heightmap *, int, int, double);
void hm_add_scale(heightmap *, heightmap *, double);
heightmap *hm_perlin_noise(int, int, vec3_field *, double);

#endif
