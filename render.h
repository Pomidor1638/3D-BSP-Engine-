#pragma once
#include "r_bsp.h"

extern vec_t GetFrustumDist(vec_t FOV) {
	return 1 / tan(radians(FOV / 2));
}
