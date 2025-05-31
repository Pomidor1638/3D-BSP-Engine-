#include "world.h"
#include "sv_model.h"

#define STOP_EPSILON 0.01

int SV_PointInHull(bspmap_t* tree, int node_num, vec3_t p) {
	while (node_num >= 0) {
		const plane_s& pl = tree->dplanes[tree->dclipnodes[node_num].planenum];
		bool side = pl.DistToPlane(p) < 0;
		node_num = tree->dclipnodes[node_num].children[side];
	}
	return node_num;
}

bool SV_RecursiveHullCheck(bspmap_t* tree, int node_num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_s* trace) {
	if (node_num < 0) {
		if (node_num != CONTENTS_SOLID){
			trace->allsolid = false;
		}
		else
			trace->startsolid = true;
		return true;		
	}

	dclipnode_t* node;
	plane_s*    plane;
	float	   t1, t2;
	float	     frac;
	int		        i;
	vec3_t	      mid;
	int          side;
	float        midf;

	node = &tree->dclipnodes[node_num];
	plane = &tree->dplanes[node->planenum];

	t1 = plane->DistToPlane(p1);
	t2 = plane->DistToPlane(p2);

	if (t1 >= 0 && t2 >= 0)
		return SV_RecursiveHullCheck(tree, node->children[0], p1f, p2f, p1, p2, trace);
	if (t1 <  0 && t2 <  0)
		return SV_RecursiveHullCheck(tree, node->children[1], p1f, p2f, p1, p2, trace);

	if (t1 < 0)
		frac = (t1 + DIST_EPSILON) / (t1 - t2);
	else
		frac = (t1 - DIST_EPSILON) / (t1 - t2);

	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;

	midf = p1f + (p2f - p1f) * frac;
	mid = p1 + frac * (p2 - p1);

	side = (t1 < 0);

	// move up to the node
	if (!SV_RecursiveHullCheck(tree, node->children[side], p1f, midf, p1, mid, trace))
		return false;


	if (SV_PointInHull(tree, node->children[side ^ 1], mid) != CONTENTS_SOLID)
		return SV_RecursiveHullCheck(tree, node->children[side ^ 1], midf, p2f, mid, p2, trace);

	if (trace->allsolid)
		return false;		// never got out of the CONTENTS_SOLID area

	//==================
	// the other side of the node is CONTENTS_SOLID, this is the impact point
	//==================

	if (!side) {
		trace->plane = { plane->normal, plane->dist };
	}
	else {
		trace->plane = { -plane->normal, -plane->dist };
	}

	while (SV_PointInHull(tree, tree->dmodels[0].headnode[1], mid) == CONTENTS_SOLID)
	{ // shouldn't really happen, but does occasionally
		frac -= 0.1;
		if (frac < 0)
		{
			trace->fraction = midf;
			trace->endpos = mid;
			return false;
		}
		midf = p1f + (p2f - p1f) * frac;
		mid = p1 + frac * (p2 - p1);

	}

	trace->fraction = midf;
	trace->endpos = mid;

	return false;
}

trace_s SV_Move(bspmap_s *tree, vec3_t start, vec3_t end) {

	trace_s trace{};

	trace.fraction = 1;
	trace.allsolid = true;
	trace.endpos = end;

	SV_RecursiveHullCheck(tree, 0, 0, 1, start, end, &trace);

	return trace;
}

int ClipVelocity(const vec3_t& in, const vec3_t& normal, vec3_t& out, float overbounce) {
	float	backoff;
	float	change;
	int		i, blocked;

	blocked = 0;
	if (normal[2] > 0)
		blocked |= 1;		// floor
	if (!normal[2])
		blocked |= 2;		// step

	backoff = (in * normal) * overbounce;

	for (i = 0; i < 3; i++) {
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}


void SV_Accelerate(float wishspeed, vec3_t wishdir, vec3_t& velocity, float dt)
{
	int			i;
	float		addspeed, accelspeed, currentspeed;
	float accelerate = 20;

	currentspeed = velocity * wishdir;
	addspeed = wishspeed - currentspeed;

	if (addspeed <= 0)
		return;
	accelspeed = accelerate * wishspeed * dt;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		velocity[i] += accelspeed * wishdir[i];
}

