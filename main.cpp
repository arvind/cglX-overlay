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

#include <stdlib.h>
#include <stdio.h>
#include <list.h>
#include <string.h>
#include <json/json.h>
#include "bmp.h"
#include "overlay.h"
#include "tile.h"
#include "main.h"

static Overlay * overlay;

static std::list<Tile> obj_list;
typedef std::list<Tile>::iterator obj_iter;

static int state;

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
	glMatrixMode(GL_PROJECTION);
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
	for(it = obj_list.begin(); it != obj_list.end(); it++)
		it->drawTile();

	overlay->drawOverlay();
	glutSwapBuffers();

	glutPostRedisplay();
}

void handleMouseClick(GLint button, GLint button_state, GLint x, GLint y) {
	if (cglXStartUpdate()) {
	    if (button_state == GLUT_UP) {
	        state = 0;
	        overlay->removeFinger(0);
	        return;
	    }

        if (button == GLUT_LEFT_BUTTON)
            state |= PAN;

        if (button == GLUT_RIGHT_BUTTON)
            state |= ZOOM;

	//  These coordinates are calculated relative to the overlay
	    GLfloat normalized_x = (GLfloat) x / getHeadWidth();
	    GLfloat normalized_y = (GLfloat) y / getHeadHeight();
	    overlay->addFinger(0, normalized_x, normalized_y, 0.05, 0.05);

	    GLfloat posInOverlay[2];
	    overlay->getPosOfFinger(0, posInOverlay);

	    GLint modifiers = glutGetModifiers();

	    obj_iter it, intersected_tile;
	    bool intersected = false;
	    for(it = obj_list.begin(); it != obj_list.end(); it++) {
	        if(it->intersects(posInOverlay[0], posInOverlay[1]) && button_state == GLUT_DOWN) {
	            printf("intersected %s\n", it->filename);
	            it->toggleSelected();

	            intersected_tile = it;
	            intersected = true;
	        } else {
	            printf("didn't intersected %s\n", it->filename);
	            if((modifiers & GLUT_ACTIVE_CTRL) == 0)
	                it->setSelected(false);
	        }
	    }
	    // For overlapping images, push the selected image to the end
	    // such that it renders "on top"
	    if(intersected) {
	        obj_list.erase(intersected_tile);
	        obj_list.push_back(*intersected_tile);
	    }
	}

	cglXUpdateDone();

	glutPostRedisplay();
}

void handleMouseMove(GLint x, GLint y) {
	if (cglXStartUpdate()) {
		if (state == 0)
			return;

        GLfloat oldPosition[2];
        overlay->getPosOfFinger(0, oldPosition);

        GLfloat normalized_x = (GLfloat) x / getHeadWidth();
        GLfloat normalized_y = (GLfloat) y / getHeadHeight();
        overlay->moveFinger(0, normalized_x, normalized_y);

        GLfloat newPosition[2];
        overlay->getPosOfFinger(0, newPosition);

        obj_iter it;
        for(it = obj_list.begin(); it != obj_list.end(); it++)
            if(it->isSelected())
                it->updateTransformations(state, newPosition[0], newPosition[1],
		                                         oldPosition[0], oldPosition[1]);

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
        GLfloat oldPosition[2];
        overlay->getPosOfFinger(0, oldPosition);

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

        GLfloat newPosition[2];
        overlay->getPosOfFinger(0, newPosition);

        obj_iter it;
        for(it = obj_list.begin(); it != obj_list.end(); it++)
            if(it->isSelected())
                it->updateTransformations(state, newPosition[0], newPosition[1],
                                                 oldPosition[0], oldPosition[1]);

	}
	cglXUpdateDone();

	glutPostRedisplay();
}

void handleCustomMsg(int len, char *msg) {
    if (cglXStartUpdate()) {
        GLfloat overlay_w = overlay->getWidth();
        GLfloat overlay_h = overlay->getHeight();

        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse( msg, root );
        if ( !parsingSuccessful ) {
            printf("Failed to parse configuration\n");
            printf("%s\n", reader.getFormatedErrorMessages());
            return;
        }

        std::string event = root["event"].asString();
        std::string overlayID = root["overlayID"].asString();

        if(event.compare("OVERLAY_MOVE") == 0) {
            GLfloat oldPosition[2];
            overlay->getPosOfFinger(0, oldPosition);

            double normalized_x = root["normalizedX"].asDouble();
            double normalized_y = root["normalizedY"].asDouble();

            GLfloat overlay_x = ((normalized_x * getHeadWidth())  - (getHeadWidth()  / 2)) / 100.0f;
            GLfloat overlay_y = -((normalized_y * getHeadHeight()) - (getHeadHeight() / 2)) / 100.0f;

            overlay->setOverlayPos(overlay_x, overlay_y);

            GLfloat newPosition[2];
            overlay->getPosOfFinger(0, newPosition);

            obj_iter it;
            for(it = obj_list.begin(); it != obj_list.end(); it++)
                if(it->isSelected())
                    it->updateTransformations(state, newPosition[0], newPosition[1],
                                                     oldPosition[0], oldPosition[1]);
        } else if(event.compare("OVERLAY_SCALE") == 0) {
            double scale_factor = root["scaleFactor"].asDouble();
            scale_factor = scale_factor + 1;

            overlay->setOverlaySize(overlay_w * scale_factor, overlay_h * scale_factor);
        }

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
	cglXCustomMsgFunc(handleCustomMsg);
	glutMainLoop();
	return 0; // ANSI C requires main to return int.
}
