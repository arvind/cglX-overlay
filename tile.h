/*
 * tile.h
 *
 *  Created on: Jul 19, 2010
 *      Author: root
 */

#ifdef DEF_USE_CGLX
#ifdef __APPLE__
	#include <cglX/cglX.h>
#else
	#include "cglX.h"
#endif
	using namespace cglx;
#else
	#include <GL/glut.h>
#endif

#ifndef TILE_H_
#define TILE_H_

class Tile {
	public:
		GLuint  texture;
		char *  filename;

		GLfloat dim[2];		// Dimension
		GLfloat trans[3];	// Translation
		GLfloat rot[2];		// Rotation

		bool is_selected;

	public:
		Tile(char * filename, float trans_x, float trans_y, float trans_z, float dim_x, float dim_y);

		void loadTextures();
		void drawTile();
		void updateTransformations(GLint state, GLint x, GLint y, GLint ox, GLint oy);
		bool intersects(GLfloat mouse_x, GLfloat mouse_y);

		void setSelected(bool s);
};

#endif /* TILE_H_ */
