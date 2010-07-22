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

		GLfloat x, y, z;       // Translation
		GLfloat width, height; // Dimensions
		GLfloat rot_x, rot_y;  // Rotation

		bool is_selected;

	public:
		Tile(char * filename, float x, float y, float z, float width, float height);

		void loadTextures();
		void drawTile();
		void updateTransformations(GLint state, GLint x, GLint y, GLint ox, GLint oy, GLfloat* boundingBox);
		bool intersects(GLfloat mouse_x, GLfloat mouse_y);

		void setSelected(bool s);
};

#endif /* TILE_H_ */
