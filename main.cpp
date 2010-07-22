#include "main.h"

void init(void) {
	Tile t0("Data/0.bmp", -1.5f, -1.2f, -6.0f, 2.0, 2.0);
	Tile t1("Data/1.bmp",  1.5f, -1.2f, -6.0f, 2.0, 2.0);
	Tile t2("Data/2.bmp",  0.0f,  1.2f, -6.0f, 2.0, 1.5);

	obj_list.push_back(t0);
	obj_list.push_back(t1);
	obj_list.push_back(t2);

	GLfloat overlay_color[3] = {1.0f, 0.0f, 0.0f};
	overlay = new Overlay(0.0f, 0.0f, -6.0f, 2.0f, 1.4f, overlay_color);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glShadeModel(GL_SMOOTH);
}

void handleReshape(GLint w, GLint h) {
	float ratio;

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).

	if (h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the clipping volume
#ifdef DEF_USE_CGLX
	cglXPerspective(45, ratio, 0.1, 1000);
#else
	gluPerspective(45, ratio, 0.1, 1000);
#endif

	// setting the camera now
	glMatrixMode( GL_MODELVIEW);
}

void handleDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	obj_iter it;
	int i = 0;
	for(it = obj_list.begin(); it != obj_list.end(); it++, i++)
		it->drawTile();

	overlay->drawOverlay();
	glutSwapBuffers();

	glutPostRedisplay();
}

void handleMouseClick(GLint button, GLint button_state, GLint x, GLint y) {
	if (cglXStartUpdate()) {
	    mouse_x = x;
	    mouse_y = y;

	    if (button_state == GLUT_UP) {
	        state = 0;
	        if(chosen_obj != NULL) {
	            chosen_obj->setSelected(false);
	            chosen_obj = NULL;
	        }
	        overlay->removeFinger(0);
	        return;
	    }

	//  These coordinates should be used to translate actual mouse coordinates
	//  if the mouse is used on the head
	//
	//  GLfloat gl_x =  ((GLfloat) x - (getHeadWidth()  / 2)) / 100;
	//  GLfloat gl_y = -((GLfloat) y - (getHeadHeight() / 2)) / 100;
	//
	//  These coordinates are calculated relative to the overlay
	    GLfloat normalized_x = (GLfloat) x / getHeadWidth();
	    GLfloat normalized_y = (GLfloat) y / getHeadHeight();
	    overlay->addFinger(0, normalized_x, normalized_y, 0.05, 0.05);

	    GLfloat posInOverlay[2];
	    overlay->getPosOfFinger(0, posInOverlay);

	    obj_iter it;
	    for(it = obj_list.begin(); it != obj_list.end(); it++) {
	//      if(it->intersects(gl_x, gl_y) && button_state == GLUT_DOWN) {
	        if(it->intersects(posInOverlay[0], posInOverlay[1]) && button_state == GLUT_DOWN) {
	            it->setSelected(true);

	            // For overlapping images, push the selected image to the end
	            // such that it renders "on top"
	            obj_list.erase(it);
	            obj_list.push_back(*it);

	            chosen_obj = &*it;
	        } else {
	            it->setSelected(false);
	        }
	    }

	    if (button == GLUT_LEFT_BUTTON)
	        state |= PAN;

	    if (button == GLUT_RIGHT_BUTTON)
	        state |= ZOOM;
	}

	cglXUpdateDone();

	glutPostRedisplay();
}

void handleMouseMove(GLint x, GLint y) {
	if (cglXStartUpdate()) {
		if (state == 0 || chosen_obj == NULL)
			return;

		GLint omx, omy;

		omx = mouse_x;
		omy = mouse_y;

		mouse_x = x;
		mouse_y = y;

        GLfloat normalized_x = (GLfloat) x / getHeadWidth();
        GLfloat normalized_y = (GLfloat) y / getHeadHeight();
        overlay->moveFinger(0, normalized_x, normalized_y);

		GLfloat bounding_box[4];
		overlay->getBoundingBox(bounding_box);

		if(chosen_obj != NULL)
		    chosen_obj->updateTransformations(state, mouse_x, mouse_y, omx, omy, bounding_box);
	}
	cglXUpdateDone();

	glutPostRedisplay();
}

void handleSpecKeys(GLint key, GLint x, GLint y) {
	GLfloat overlay_x = overlay->getX();
	GLfloat overlay_y = overlay->getY();
	GLfloat overlay_w = overlay->getWidth();
	GLfloat overlay_h = overlay->getHeight();

	if (cglXStartUpdate()) {
		switch (key) {
		case GLUT_KEY_LEFT:
			overlay->setOverlayPos(overlay_x - 0.1f, overlay_y);
			break;

		case GLUT_KEY_RIGHT:
			overlay->setOverlayPos(overlay_x + 0.1f, overlay_y);
			break;

		case GLUT_KEY_UP:
			overlay->setOverlayPos(overlay_x, overlay_y + 0.1f);
			break;

		case GLUT_KEY_DOWN:
			overlay->setOverlayPos(overlay_x, overlay_y - 0.1f);
			break;

		case GLUT_KEY_PAGE_UP:
			overlay->setOverlaySize(overlay_w * 1.05, overlay_h * 1.05);
			break;

		case GLUT_KEY_PAGE_DOWN:
			overlay->setOverlaySize(overlay_w * 0.95, overlay_h * 0.95);
			break;
		}

        GLfloat bounding_box[4];
        overlay->getBoundingBox(bounding_box);

        if(chosen_obj != NULL)
            chosen_obj->updateTransformations(PAN, mouse_x, mouse_y, mouse_x, mouse_y, bounding_box);
	}
	cglXUpdateDone();

	glutPostRedisplay();
}

void handleXEvent(XEvent * event) {
	if (cglXStartUpdate()) {
		printf("event type: %p\n", event);
	}
	cglXUpdateDone();

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300, 300);
	glutCreateWindow("Simple Photo Application");
	init();
	glutDisplayFunc(handleDisplay);
	glutIdleFunc(handleDisplay);
	glutReshapeFunc(handleReshape);
	glutMouseFunc(handleMouseClick);
	glutMotionFunc(handleMouseMove);
	glutSpecialFunc(handleSpecKeys);
	//cglXXEventFunc(handleXEvent);
	glutMainLoop();
	return 0; // ANSI C requires main to return int.
}
