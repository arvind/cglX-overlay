/*
 * tile.cpp
 *
 *  Created on: Jul 19, 2010
 *      Author: root
 */

#include "main.h";
#include "tile.h";
#include "bmp.h";

Tile::Tile(char * f, float trans_x, float trans_y, float trans_z, float dim_x, float dim_y) {
	this->filename = f;

	this->trans[0] = trans_x;
	this->trans[1] = trans_y;
	this->trans[2] = trans_z;

	this->dim[0]   = dim_x;
	this->dim[1]   = dim_y;

	this->rot[0]   = 0;
	this->rot[1]   = 0;

	this->is_selected = false;

	this->loadTextures();
}

void Tile::loadTextures() {
	BMPClass bmp;
	BMPLoad(this->filename, bmp);

	glGenTextures(1, &(this->texture)); // Create The Texture

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp.width, bmp.height, 0, GL_RGB,
	GL_UNSIGNED_BYTE, bmp.bytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Tile::drawTile() {
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(this->trans[0], this->trans[1], this->trans[2]);
	glRotatef(this->rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(this->rot[1], 0.0f, 1.0f, 0.0f);

	GLfloat dim_x = this->dim[0] / 2;
	GLfloat dim_y = this->dim[1] / 2;

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-dim_x, dim_y, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( dim_x, dim_y, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( dim_x,-dim_y, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-dim_x,-dim_y, 0.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	if(this->is_selected) { // Outline
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth( 2.0f );
		glBegin(GL_QUADS);
			glVertex3f(-dim_x, dim_y, 0.0f);
			glVertex3f( dim_x, dim_y, 0.0f);
			glVertex3f( dim_x,-dim_y, 0.0f);
			glVertex3f(-dim_x,-dim_y, 0.0f);
		glEnd();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}

void Tile::updateTransformations(GLint state, GLint x, GLint y, GLint ox, GLint oy) {
	int dx = ox - x;
	int dy = y - oy;

	printf("state: %d dx: %d dy: %d\t\t", state, dx, dy);

	switch (state) {
		case PAN:
			this->trans[0] -= dx / 100.0f;
			this->trans[1] -= dy / 100.0f;
		break;
		case ROTATE:
			this->rot[0] += (dy * 180.0f) / 500.0f;
			this->rot[1] -= (dx * 180.0f) / 500.0f;

#define clamp(x) x = x > 360.0f ? x-360.0f : x < -360.0f ? x+=360.0f : x
			clamp(this->rot[0]);
			clamp(this->rot[1]);
		break;
		case ZOOM:
			this->trans[2] -= (dx + dy) / 100.0f;
		break;
	}

	printf("trans[0]: %f trans[1]: %f\n", trans[0], trans[1]);

}

bool Tile::intersects(GLfloat mouse_x, GLfloat mouse_y) {
	printf("\n\nfilename: %s\nmouse_x: %f trans[0]: %f dim[0]: %f\nmouse_y: %f trans[1]: %f dim[1]: %f",
			filename, mouse_x, trans[0], dim[0], mouse_y, trans[1], dim[1]);

	if(mouse_x > this->trans[0] - (this->dim[0] / 2) && mouse_x < this->trans[0] + (this->dim[0] / 2) &&
	   mouse_y > this->trans[1] - (this->dim[1] / 2) && mouse_y < this->trans[1] + (this->dim[1] / 2))
			return true;

	return false;
}

void Tile::setSelected(bool s) {
	this->is_selected = s;
}
