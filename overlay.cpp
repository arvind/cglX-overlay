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
#include "overlay.h";

Overlay::Overlay(std::string id, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat * color) {
    this->id = id;
	this->setOverlay(x, y, w, h);
	this->z = z;
	this->color[0] = color[0];
	this->color[1] = color[1];
	this->color[2] = color[2];
	this->sphere_size = 0.1f;
}

std::string Overlay::getID() {
    return this->id;
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

GLfloat Overlay::getSphereSize() {
    return this->sphere_size;
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
	glTranslatef(this->x, this->y, this->z);
	
	// First draw overlay fill
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	this->drawOverlayShape();
	
	// Then outline it
	glColor4f(this->color[0], this->color[1], this->color[2], 0.4f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth( 3.0f );
	this->drawOverlayShape();
	
	// Draw fingers
	for(finger_iter it = this->fingers.begin(); it != this->fingers.end(); it++) {
	    it->second->drawFinger();
	}

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


void Overlay::getBoundingBox(GLfloat * bounding_box) {
    // [0] - minX
    // [1] - minY
    // [2] - maxX
    // [3] - maxY

    bounding_box[0] = this->x - (this->width  / 2);
    bounding_box[1] = this->y - (this->height / 2);

    bounding_box[2] = this->x + (this->width  / 2);
    bounding_box[3] = this->y + (this->height / 2);
}

void Overlay::getBoundingBoxWithSOI(GLfloat * bounding_box) {
    // [0] - minX
    // [1] - minY
    // [2] - maxX
    // [3] - maxY

    bounding_box[0] = this->x - (this->width  / 2) - this->sphere_size;
    bounding_box[1] = this->y - (this->height / 2) - this->sphere_size;

    bounding_box[2] = this->x + (this->width  / 2) + this->sphere_size;
    bounding_box[3] = this->y + (this->height / 2) + this->sphere_size;
}

void Overlay::addFinger(std::string id, GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
    Finger * f = new Finger(this, x, y, width, height);
    this->fingers.insert(std::pair<std::string, Finger*>(id, f));
}

void Overlay::removeFinger(std::string id) {
    if(this->fingers.size() < 1)
        return;

    finger_iter it = this->fingers.find(id);
    if(it->first == id)
        this->fingers.erase(it);
}

void Overlay::moveFinger(std::string id, GLfloat x, GLfloat y) {
    finger_iter it = this->fingers.find(id);
    if(it->first == id) {
        GLfloat f_width = it->second->getWidth();
        GLfloat f_height = it->second->getHeight();
        this->fingers.erase(it);

        this->addFinger(id, x, y, f_width, f_height);
    }
}

void Overlay::getPosOfFinger(std::string id, GLfloat * posInOverlay) {
    if(this->fingers.size() < 1)
        return;

    finger_iter it = this->fingers.find(id);
    if(it->first == id)
        it->second->getPosInOverlay(posInOverlay);
}

Overlay::Finger::Finger(Overlay* const overlay, GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
    this->overlay = overlay;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

GLfloat Overlay::Finger::getX() {
    return this->x;
}

GLfloat Overlay::Finger::getY() {
    return this->y;
}

GLfloat Overlay::Finger::getWidth() {
    return this->width;
}

GLfloat Overlay::Finger::getHeight() {
    return this->height;
}

//-------------------------------------------------------------------------
//  Draws an oval centered at (x_center, y_center)
//
// The oval is bound inside a rectangle whose width is w
// and height is h.
//
// n represents the number of line segments used to draw the oval.
//-------------------------------------------------------------------------
void Overlay::Finger::drawOval(GLfloat x_center, GLfloat y_center, GLfloat w, GLfloat h, GLint n)
{
    GLfloat theta, angle_increment;
    GLfloat x, y;
    if (n <= 0)
        n = 1;
    angle_increment = PI_2 / n;
    glPushMatrix ();
    glLoadIdentity();
    glTranslatef (x_center, y_center, -6.0f);
    glBegin (GL_TRIANGLE_FAN);
    for(theta = 0.0f; theta < PI_2; theta += angle_increment) {
        x = w/2 * cos (theta);
        y = h/2 * sin (theta);

        glColor3f (this->overlay->color[0], this->overlay->color[1], this->overlay->color[2]);
        glVertex2f (x, y);
    }
    glEnd ();
    glPopMatrix ();
}

void Overlay::Finger::drawFinger() {
    GLfloat posInOverlay[2];
    this->getPosInOverlay(posInOverlay);

    this->drawOval(posInOverlay[0], posInOverlay[1], this->width, this->height, 10);
}

void Overlay::Finger::getPosInOverlay(GLfloat * posInOverlay) {
    // normalized (x, y) is (0, 0) at the top left and (1, 1) at the bottom right.
    // this (x,y) is (0, 0) in the center of the overlay.
    GLfloat bounding_box[4];
    this->overlay->getBoundingBox(bounding_box);

    posInOverlay[0] = bounding_box[0] + (this->x * this->overlay->width);
    posInOverlay[1] = bounding_box[3] - (this->y * this->overlay->height);

    posInOverlay[0] = posInOverlay[0] < bounding_box[0] ? bounding_box[0] :
                        posInOverlay[0] > bounding_box[2] ? bounding_box[2] :
                            posInOverlay[0];

    posInOverlay[1] = posInOverlay[1] < bounding_box[1] ? bounding_box[1] :
                        posInOverlay[1] > bounding_box[3] ? bounding_box[3] :
                            posInOverlay[1];

//    printf("x: %f y: %f width: %f height: %f\n", x, y, width, height);
//    printf("norm x: %f norm y: %f pos x: %f pos y: %f\n", this->x, this->y, posInOverlay[0], posInOverlay[1]);
}
