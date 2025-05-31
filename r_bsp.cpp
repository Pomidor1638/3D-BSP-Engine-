#include "r_bsp.h"
#include <queue>

int nodenum;

struct {
	int numpoints;
	vec3_t points[MAX_POINTS_ON_POLYGON];
	float  data[MAX_POINTS_ON_POLYGON][2];
	bool flat;
} polygon;

void MakePolygonS(dface_t* face, bspmap_t* map) {

	int i, j;
	uint32_t* tpts;

	vec3_t vec;

	polygon.numpoints = face->numpoints;
	tpts = map->dvertextable + face->firstpoint;
	for (i = 0; i < polygon.numpoints; i++) {
		vec3_t& p = polygon.points[i];
		float* d = polygon.data[i];

		dtexinfo_t& t = map->texinfo[face->texturenum];

		d[0] = t.vecs[0][3];
		d[1] = t.vecs[1][3];

		p = map->dvertexes[tpts[i]];
		for (j = 0; j < 3; j++) {
			d[0] += p[j] * t.vecs[0][j];
			d[1] += p[j] * t.vecs[1][j];
		}
	}
}

void DrawMirror(dface_t* face, vec3_t point, bspmap_t* map, vec3_t look) {

	GLfloat reflect[16];
	GLfloat curmatrix[16];
	vec3_t mnormal = map->dplanes[face->planenum].normal;
	vec_t mdist = map->dplanes[face->planenum].dist;

	if (face->side) {
		mnormal = -mnormal;
		mdist = -mdist;
	}

	if (mnormal * point + mdist < 0)
		return;

	memset(reflect, 0, sizeof(reflect));
	for (int i = 0; i < 3; i++) {
		reflect[5 * i] = 1;
		for (int j = i; j < 3; j++) {
			reflect[4 * i + j] -= 2 * mnormal[i] * mnormal[j];
		}
	}
	reflect[15] = 1;
	/*for (int i = 0; i < 3; i++) {
		reflect[4 * i + 3] = mnormal[i] * mdist;
	}*/

	glPushMatrix();
	//glGetFloatv(GL_MODELVIEW_MATRIX, curmatrix);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glTranslatef(2 * mnormal[0] * mdist, 2 * mnormal[1] * mdist, 2 * mnormal[2] * mdist);
	//glLoadMatrixf(reflect);
	//glMultMatrixf(curmatrix);

	glMultMatrixf(reflect);
	PaintersAlgorithm(map->hullheadnum[0], map, point, look);

	glPopMatrix();
}


void DrawPolygon(dface_t* face, vec_t bright, bspmap_t* map, bool wire) {

	MakePolygonS(face, map);
	

	if (wire) {
		glPushMatrix();
			glDisable(GL_TEXTURE_2D);
			glColor3f(0, 1, 0);
			glBegin(GL_LINE_LOOP);
			glLineWidth(2);
			for (int i = 0; i < polygon.numpoints; i++) {
				const vec3_t& p = polygon.points[i];
				glVertex3f(p[0], p[1], p[2]);
			}

			glEnd();

		glPopMatrix();

	}
	else {
		glPushMatrix();

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, map->texinfo[face->texturenum].texturenum);
			

			glColor3f(bright, bright, bright);
			glBegin(GL_POLYGON);

			for (int i = 0; i < polygon.numpoints; i++) {
				const vec3_t& p = polygon.points[i];
				const float* d = polygon.data[i];
				glTexCoord2fv(d);
				glVertex3fv((const float*) & p);
			}

			glEnd();

			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
}
void DrawPortal(dportal_t* portal, int nodenum, bspmap_t* map) {
	glPushMatrix();

		glDisable(GL_TEXTURE_2D);
		glColor3f(1, 0, 0);

		glBegin(GL_LINE_LOOP);

			for (int i = 0; i < portal->numpoints; i++) {
				const vec3_t& p = map->dvertexes[map->dvertextable[portal->firstpoint + i]];
				glVertex3f(p[0], p[1], p[2]);
			}

		glEnd();

	glPopMatrix();
}

int processed;

void PaintersAlgorithm(int node_num, bspmap_t* tree, vec3_t Point, vec3_t LookVec) {

	dleaf_t cur_leaf;
	dface_t* dfaces;
	dportal_t* portal;
	dnode_t node;

	if (node_num < 0) {
		cur_leaf = tree->dleafs[-(node_num + 1)];
		dfaces = &tree->dfaces[cur_leaf.firstface];
		for (int i = 0; i < cur_leaf.numfaces; i++) {
			float bright = -(0.4) * LookVec * tree->dplanes[dfaces[i].planenum].normal;
			if (dfaces[i].side)
				bright = -bright;
			if (bright < 0)
				bright = 0; 
			bright += (0.6);
			if (dfaces[i].texturenum) {
				if (tree->texinfo[dfaces[i].texturenum].texturenum > 0) {
					DrawPolygon(&dfaces[i], bright, tree, GetKeyState('V') < 0 ? nodenum != node_num : false);
				}
				else if (processed < 10){
					processed++;
					//DrawMirror(&dfaces[i], Point, tree, LookVec);
				}
			}
		}
		if (nodenum == node_num || GetKeyState('C') < 0)
			for (int i = 0; i < cur_leaf.numportals; i++) {
				portal = &tree->dportals[tree->dportaltable[cur_leaf.firstportal + i]];
				DrawPortal(portal, -(node_num + 1), tree);
			}
		return;
	}
	node = tree->dnodes[node_num];
	


	plane_s cur_plane = tree->dplanes[node.planenum];

	vec_t t = cur_plane.DistToPlane(Point);

	bool side = t > 0;

	PaintersAlgorithm(node.children[ side], tree, Point, LookVec);
	PaintersAlgorithm(node.children[!side], tree, Point, LookVec);
}