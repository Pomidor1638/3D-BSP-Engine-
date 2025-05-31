#pragma once
#include <ctime>
#include "Core.h"
#include "mathlib.h"

#define DIST_EPSILON 0.0135


int SV_PointInHull(bspmap_t* tree, int node_num, vec3_t p);
bool SV_RecursiveHullCheck(bspmap_t* tree, int node_num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_s* trace);
int ClipVelocity(const vec3_t& in, const vec3_t& normal, vec3_t& out, float overbounce);
void SV_Accelerate(float wishspeed, vec3_t wishdir, vec3_t& velocity, float dt);

trace_s SV_Move(bspmap_s* tree, vec3_t start, vec3_t end);