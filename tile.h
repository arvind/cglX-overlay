/*
 * tile.h
 *
 *  Created on: Jul 19, 2010
 *      Author: root
 */

#ifndef TILE_H_
#define TILE_H_

// States //
#define PAN                     1
#define ZOOM                    2
#define ROTATE                  3

class Tile {
	private:
		GLuint  texture;
		char *  filename;

		GLfloat x, y, z;       // Translation
		GLfloat width, height; // Dimensions
		GLfloat rot_x, rot_y;  // Rotation

		bool is_selected;

	public:
		Tile(char * filename, float x, float y, float z, float width, float height);

		GLfloat getX();
		GLfloat getY();
		GLfloat getZ();
		GLfloat getWidth();
		GLfloat getHeight();

		void setTileSize(GLfloat new_w, GLfloat new_h);

		void loadTextures();
		void drawTile();
		void updateTransformations(GLint state, GLfloat x, GLfloat y, GLfloat ox, GLfloat oy);
		bool intersects(GLfloat mouse_x, GLfloat mouse_y);

		void setSelected(bool s);
		void toggleSelected();
		bool isSelected();
};

#endif /* TILE_H_ */
