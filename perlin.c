#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "perlin.h"

////
// Vector
////
vec3 v3_new(double x, double y, double z) {
	vec3 ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;
	return ret;
}

vec3 v3_from_angle(double angle, double angle2, double length) {
	double
		x = cos(angle) * cos(angle2),
		y = sin(angle),
		z = sin(angle2) * cos(angle);
	return v3_new(x * length, y * length, z * length);
}

vec3 v3_unit(double angle, double angle2) {
	return v3_from_angle(angle, angle2, 1);
}

double v3_length(vec3 v) {
	return sqrt(v3_dot(v, v));
}

vec3 v3_normalize(vec3 v) {
	double l = v3_length(v);
	return v3_new(v.x / l, v.y / l, v.z / l);
}

vec3 v3_negate(vec3 v) {
	return v3_new(-v.x, -v.y, -v.z);
}

vec3 v3_add(vec3 v1, vec3 v2) {
	return v3_new(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

vec3 v3_sub(vec3 v1, vec3 v2) {
	return v3_add(v1, v3_negate(v2));
}

double v3_dot(vec3 v1, vec3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

////
// Vector field
////
vec3_field *v3f_new(int width, int height, int depth) {
	vec3_field *vf = (vec3_field *) malloc(sizeof(vec3_field));
	vf->width = width;
	vf->height = height;
	vf->depth = depth;
	vf->data = (vec3 *) malloc(width * height * depth * sizeof(vec3));
	for(int i = 0; i < width * height * depth; i++) {
		vf->data[i] = v3_new(0, 0, 0);
	}
	return vf;
}

vec3_field *v3f_random_unit(int width, int height, int depth) {
	vec3_field *vf = v3f_new(width, height, depth);
	for(int i = 0; i < width * height * depth; i++) {
		double r1 = (float) rand() / (float) RAND_MAX;
		double r2 = (float) rand() / (float) RAND_MAX;
		r1 = r1 * 3.1415 * 2;
		r2 = r2 * 3.1415 * 2;
		vf->data[i] = v3_unit(r1, r2);
	}
	return vf;
}

void v3f_destroy(vec3_field *vf) {
	free(vf->data);
	free(vf);
}

vec3 v3f_get(vec3_field *vf, int x, int y, int z) {
	int wh = vf->width * vf->height;
	return vf->data[z * wh + y * vf->width + x];
}

void v3f_set(vec3_field *vf, int x, int y, int z, vec3 v) {
	int wh = vf->width * vf->height;
	vf->data[z * wh + y * vf->width + x] = v;
}

////
// Heightmap
////
heightmap *hm_new(int width, int height) {
	heightmap *hm = (heightmap *) malloc(sizeof(heightmap));
	hm->width = width;
	hm->height = height;
	hm->data = (double *) malloc(width * height * sizeof(double));
	for(int i = 0; i < width * height; i++) {
		hm->data[i] = 0;
	}
	return hm;
}

void hm_destroy(heightmap *hm) {
	free(hm->data);
	free(hm);
}

double hm_get(heightmap *hm, int x, int y) {
	return hm->data[y * hm->width + x];
}

void hm_set(heightmap *hm, int x, int y, double v) {
	hm->data[y * hm->width + x] = v;
}

void hm_add_scale(heightmap *hm1, heightmap *hm2, double ratio) {
	if(hm1->width != hm2->width || hm1->height != hm2->height) {
		return;
	}
	for(int i = 0; i < hm1->width * hm1->height; i++) {
		hm1->data[i] = hm1->data[i] + hm2->data[i] * ratio;
	}
}

static double __curve(double t)
	{return 3 * t*t - 2 * t*t*t;}
static double __interpolate(double t, double x, double y)
	{t = __curve(t); return (1 - t) * x + t * y;}
heightmap *hm_perlin_noise(int width, int height, vec3_field *vf, double layer) {
	heightmap *hm = hm_new(width, height);
	int vfw = vf->width - 1;
	int vfh = vf->height - 1;
	int tileWidth = width / vfw;
	int tileHeight = height / vfh;
	double relZ = layer;
	for(int y = 0; y < height; y++) for(int x = 0; x < width; x++) {
		int tileX = x / tileWidth;
		int tileY = y / tileHeight;
		double relX = (double)(x % tileWidth) / tileWidth;
		double relY = (double)(y % tileHeight) / tileHeight;
		vec3
			v000 = v3_normalize(v3_new(relX    , relY    , relZ    )),
			v100 = v3_normalize(v3_new(relX - 1, relY    , relZ    )),
			v010 = v3_normalize(v3_new(relX    , relY - 1, relZ    )),
			v110 = v3_normalize(v3_new(relX - 1, relY - 1, relZ    )),
			v001 = v3_normalize(v3_new(relX    , relY    , relZ - 1)),
			v101 = v3_normalize(v3_new(relX - 1, relY    , relZ - 1)),
			v011 = v3_normalize(v3_new(relX    , relY - 1, relZ - 1)),
			v111 = v3_normalize(v3_new(relX - 1, relY - 1, relZ - 1));
		vec3
			c000 = v3f_get(vf, tileX    , tileY    , 0),
			c100 = v3f_get(vf, tileX + 1, tileY    , 0),
			c010 = v3f_get(vf, tileX    , tileY + 1, 0),
			c110 = v3f_get(vf, tileX + 1, tileY + 1, 0),
			c001 = v3f_get(vf, tileX    , tileY    , 1),
			c101 = v3f_get(vf, tileX + 1, tileY    , 1),
			c011 = v3f_get(vf, tileX    , tileY + 1, 1),
			c111 = v3f_get(vf, tileX + 1, tileY + 1, 1);
		double
			n000 = v3_dot(v000, c000),
			n100 = v3_dot(v100, c100),
			n010 = v3_dot(v010, c010),
			n110 = v3_dot(v110, c110),
			n001 = v3_dot(v001, c001),
			n101 = v3_dot(v101, c101),
			n011 = v3_dot(v011, c011),
			n111 = v3_dot(v111, c111);
		double
			nx00 = __interpolate(relX, n000, n100),
			nx10 = __interpolate(relX, n010, n110),
			nx01 = __interpolate(relX, n001, n101),
			nx11 = __interpolate(relX, n011, n111);
		double
			nxy0 = __interpolate(relY, nx00, nx10),
			nxy1 = __interpolate(relY, nx01, nx11);
		double
			nxyz = __interpolate(relZ, nxy0, nxy1);
		hm_set(hm, x, y, nxyz);
	}
	return hm;
}
