#include "sv_model.h"
#include <iostream>


void entity::Rotate(vec3_t d_angle) {
	if ((d_angle[0] + angle[0] < M_PI) and (d_angle[0] + angle[0] > 0))
		angle[0] = angle[0] + d_angle[0];
	else
		angle[0] = ((d_angle[0] + angle[0] < M_PI) and (d_angle[0] + angle[0] > 0)) ? (angle[0] + d_angle[0]) : (
			(d_angle[0] + angle[0] <= 0) ? 0 : M_PI
			);

	angle[1] = mod(angle[1] + d_angle[1], 2 * M_PI);
	angle[2] = mod(angle[2] + d_angle[2], 2 * M_PI);
};

vec3_t GetLookVector(vec3_t angle) {

	vec_t cos_x = cos(angle[0]);
	vec_t sin_x = sin(angle[0]);

	vec_t cos_z = cos(angle[2]);
	vec_t sin_z = sin(angle[2]);

	return vec3_t{ -sin_z * sin_x, cos_z * sin_x, -cos_x };
}

