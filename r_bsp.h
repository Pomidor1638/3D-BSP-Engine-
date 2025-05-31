#pragma once
#include "Core.h"
#include "sv_model.h"

extern int nodenum;
extern int processed;

void DrawPolygon(dface_t* face, vec_t bright, bspmap_t* map, bool wire);
void DrawPortal(dportal_t* portal, int nodenum, bspmap_t* map);
void PaintersAlgorithm(int nodenum, bspmap_t* tree, vec3_t Point, vec3_t LookVec);

typedef struct {
	int prevleaf;
	int numplanes;
	plane_s planes[MAX_POINTS_ON_POLYGON];

} frustum_t;

void Draw(vec3_t point, bspmap_t* map, vec3_t look, bool wire);