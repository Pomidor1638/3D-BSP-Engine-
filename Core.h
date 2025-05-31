#pragma once
#include <windows.h>
#include <gl/gl.h>
#include "sv_model.h"
#include <string>
#include <vector>
#include <iostream>

#pragma comment(lib, "opengl32.lib")


//#define SAFE_VERSION

extern void Game_Init();
#ifdef  SAFE_VERSION
extern GLuint Texture[4];
#else 
extern GLuint Texture[150];
#endif //  SAFE_VERSION

void ShowVec3(vec3_t p);
void LoadTexturesToGPU();
bool LoadBSPtree(std::string filename, bspmap_t* tree);


