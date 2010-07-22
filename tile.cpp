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

	this->x = trans_x;
	this->y = trans_y;
	this->z = trans_z;

	this->width   = dim_x;
	this->height   = dim_y;

	this->rot_x   = 0;
	this->rot_y   = 0;

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

	glTranslatef(this->x, this->y, this->z);
	glRotatef(this->rot_x, 1.0f, 0.0f, 0.0f);
	glRotatef(this->rot_y, 0.0f, 1.0f, 0.0f);

	GLfloat dim_x = this->width / 2;
	GLfloat dim_y = this->height / 2;

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

void Tile::updateTransformations(GLint state, GLint x, GLint y, GLint ox, GLint oy, GLfloat* boundingBox) {
	int dx = ox - x;
	int dy = y - oy;

//	printf("state: %d dx: %d dy: %d\t\t", state, dx, dy);

	switch (state) {
		case PAN:
			this->x -= dx / 100.0f;
//			this->x  = this->x < boundingBox[0] ? boundingBox[0] :
//			                      this->x > boundingBox[2] ? boundingBox[2] :
//			                          this->x;

			this->y -= dy / 100.0f;
//			this->y  = this->y < boundingBox[1] ? boundingBox[1] :
//			                      this->y > boundingBox[3] ? boundingBox[3] :
//			                          this->y;
		break;
		case ROTATE:
			this->rot_x += (dy * 180.0f) / 500.0f;
			this->rot_y -= (dx * 180.0f) / 500.0f;

#define clamp(x) x = x > 360.0f ? x-360.0f : x < -360.0f ? x+=360.0f : x
			clamp(this->rot_x);
			clamp(this->rot_y);
		break;
		case ZOOM:
			this->width  *= (dx + dy) / 100.0f;
			this->height  *= (dx + dy) / 100.0f;
		break;
	}

//	printf("x: %f y: %f\n", this->x, this->y);

}

bool Tile::intersects(GLfloat mouse_x, GLfloat mouse_y) {
//	printf("\n\nfilename: %s\nmouse_x: %f x: %f width: %f\nmouse_y: %f y: %f height: %f",
//			filename, mouse_x, x, width, mouse_y, y, height);

	if(mouse_x > this->x - (this->width / 2) && mouse_x < this->x + (this->width / 2) &&
	   mouse_y > this->y - (this->height / 2) && mouse_y < this->y + (this->height / 2))
			return true;

	return false;
}

void Tile::setSelected(bool s) {
	this->is_selected = s;
}
