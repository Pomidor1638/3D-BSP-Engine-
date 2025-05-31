#pragma once
#include "mathlib.h"
#include <vector>

#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2


enum space_ñlassification {
	COINCIDENT, IN_BACK_OF,
	IN_FRONT_OF, SPANNING
};

#define	HEADER_LUMPS	  13
#define MAX_MAP_HULLS      4

typedef unsigned char byte;

typedef struct {
	float		mins[3], maxs[3];
	float		origin[3];
	int			headnode[MAX_MAP_HULLS];
	int			visleafs;
	int			firstface, numfaces;
} dmodel_t;

typedef struct {
	int		fileofs, filelen;
} lump_t;

typedef struct {
	int			version;
	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

typedef struct {

	short planenum;
	bool side;
	short texturenum;

	unsigned int firstpoint;
	unsigned short numpoints;

} dface_t;

typedef struct {

	short planenum;

	short leafs[2];

	unsigned int firstpoint;
	unsigned short numpoints;

} dportal_t;

typedef struct {

	float mins[3];
	float maxs[3];

	short planenum;
	short children[2];

} dnode_t;

typedef struct {

	short planenum;
	short children[2];

} dclipnode_t;

typedef struct {
	short			contents;

	float		mins[3];
	float		maxs[3];

	unsigned short	firstface;
	unsigned short	numfaces;

	unsigned short firstportal;
	unsigned short numportals;

} dleaf_t;

typedef struct {

	float vecs[2][4];
	int flags;
	int texturenum;

} dtexinfo_t;


typedef struct bspmap_s {
	int hullheadnum[MAX_MAP_HULLS]{ -1, -1, -1, -1 };

	int			    nummodels      = 0;
	dmodel_t*	    dmodels	       = nullptr;
				    
	int			    lightdatasize  = 0;
	byte*		    dlightdata     = nullptr;
				    
	int			    entdatasize    = 0;
	char*		    dentdata       = nullptr;
				    
	int			    numleafs       = 0;
	dleaf_t*	    dleafs         = nullptr;
				    			    
	int			    numplanes	   = 0;
	plane_s*	    dplanes        = nullptr;
				    			    
	int			    numnodes	   = 0;
	dnode_t*	    dnodes		   = nullptr;
				    			 
	int			    numdtexinfo    = 0;
	dtexinfo_t*	    texinfo	       = nullptr;
				    
	int			    numvert	       = 0;
	vec3_t*	        dvertexes      = nullptr;
				 
	int			    numvertextable = 0;
	unsigned int*   dvertextable   = nullptr;
				 
	int			    numfaces       = 0;
	dface_t*	    dfaces         = nullptr;
				    			    
	int			    numportals     = 0;
	dportal_t*      dportals       = nullptr;

	int			    numportaltable = 0;
	unsigned short* dportaltable   = nullptr;
				    			    
	int			    numclipnodes   = 0;
	dclipnode_t*    dclipnodes     = nullptr;
	


} bspmap_t;

#define MAX_POINTS_ON_POLYGON 64

struct trace_s {
	bool allsolid;	
	bool startsolid; 

	vec_t fraction;		
	vec3_t	endpos;			
	plane_s	plane;			
};

class entity {
private:
	vec3_t pos;
	vec3_t angle;
	vec3_t velocity;
	vec_t  free_time;
	vec3_t maxs, mins;
	vec_t  radius;
public:
	bool floor = false;
	void SetPos(const vec3_t& pos) { this->pos = pos; }
	void SetAngle(const vec3_t& angle) { this->angle = angle; }
	void SetVel(const vec3_t& vel) { velocity = vel; }
	vec3_t GetPos() { return pos; }
	vec3_t GetAngle() { return angle; }
	vec3_t GetVel() { return velocity; }
	void Rotate(vec3_t d_angle);
	entity(vec3_t pos, vec3_t angle) {
		SetAngle(angle);
		SetPos  (pos  );
		SetVel({ 0, 0, 0 });
	}
};

extern vec3_t GetLookVector(vec3_t angle);
