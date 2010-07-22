#include "overlay.h";

Overlay::Overlay(void) {
	this->setOverlay(0.0f, 0.0f, 2.0f, 1.4f);
	this->z = 0.0f;
//	this->drawOverlay();
}

Overlay::Overlay(GLfloat z) {
	this->setOverlay(0.0f, 0.0f, 2.0f, 1.4f);
	this->z = z;
}

Overlay::Overlay(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h) {
	this->setOverlay(x, y, w, h);
	this->z = z;
//	this->drawOverlay();
}

GLfloat Overlay::getX() {
	return this->x;
}

GLfloat Overlay::getY() {
	return this->y;
}

GLfloat Overlay::getZ() {
	return this->z;
}

GLfloat Overlay::getWidth() {
	return this->width;
}

GLfloat Overlay::getHeight() {
	return this->height;
}

void Overlay::drawOverlayShape(void) {
	GLfloat w = this->width / 2;
	GLfloat h = this->height / 2;
	
	glBegin(GL_QUADS);
		glVertex3f(-w, h, 0.0f);
		glVertex3f( w, h, 0.0f);
		glVertex3f( w,-h, 0.0f);
		glVertex3f(-w,-h, 0.0f);
	glEnd();
}

void Overlay::drawOverlay(void) {
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(this->x, this->y, -6.0f);
	
	// First draw overlay fill
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	this->drawOverlayShape();
	
	// Then outline it
	glColor4f(1.0f, 0.0f, 0.0f, 0.4f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth( 3.0f );
	this->drawOverlayShape();
	
	glPopName();
	glPopMatrix();
	
	// Now reset
	glDisable(GL_BLEND);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Overlay::setOverlay(GLfloat new_x, GLfloat new_y, GLfloat new_w, GLfloat new_h) {
	this->x = new_x;
	this->y = new_y;
	this->width = new_w;
	this->height = new_h;
}

void Overlay::setOverlaySize(GLfloat new_w, GLfloat new_h) {
	this->setOverlay(this->x, this->y, new_w, new_h);
}

void Overlay::setOverlayPos(GLfloat new_x, GLfloat new_y) {
	this->setOverlay(new_x, new_y, this->width, this->height);
}


void Overlay::getPosInOverlay(GLfloat normalized_x, GLfloat normalized_y, GLfloat * posInOverlay) {
	// normalized (x, y) is (0, 0) at the top left and (1, 1) at the bottom right.
	// this (x,y) is (0, 0) in the center of the overlay.

	posInOverlay[0] = (this->x - (this->width / 2))  + (normalized_x * this->width);
	posInOverlay[1] = (this->y + (this->height / 2));

	if(normalized_y < 0.5)
		posInOverlay[1] += normalized_y * this->height;

	if(normalized_y > 0.5)
		posInOverlay[1] -= normalized_y * this->height;

	printf("x: %f y: %f width: %f height: %f\n", x, y, width, height);
	printf("norm x: %f norm y: %f pos x: %f pos y: %f\n", normalized_x, normalized_y, posInOverlay[0], posInOverlay[1]);
}
