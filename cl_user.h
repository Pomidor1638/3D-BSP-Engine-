#pragma once
#include "Core.h"
#include "sv_model.h"


void Movement(entity* p, vec_t dt, bspmap_t* tree);
void MoveCamera(vec3_t angle, vec3_t pos);
void MovePlayerLook(entity* p, const HWND& hwnd, unsigned int width, unsigned int height);
vec3_t GetLookVector(vec3_t angle);