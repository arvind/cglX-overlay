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
#include "overlay_manager.h"
#include "tile.h"
#include "main.h"

static OverlayManager overlay_manager;

static std::list<Tile> obj_list;
typedef std::list<Tile>::iterator obj_iter;

static int state;

void init(void) {
	Tile t0("Data/zebra.bmp", -2.0f, -1.10f, -6.0f, 2.5, 1.875);
	Tile t3("Data/lion.bmp", 0.3f, -1.45f, -6.0f, 2.0, 1.5);
	Tile t1("Data/tiger.bmp",  2.1f, -0.8f, -6.0f, 2.0, 1.5);
	Tile t4("Data/giraffe.bmp", -2.0f, 1.1f, -6.0f, 2.0, 1.75);
	Tile t2("Data/elephant.bmp",  0.5f,  1.5f, -6.0f, 2.5, 1.875);
	Tile t5("Data/orangutan.bmp", 2.25f, 0.5f, -6.0f, 2.0, 1.5);
	Tile t6("Data/tucan.bmp", -0.4f, 0.0f, -6.0f, 2.0, 1.5);
	Tile t7("Data/cheetah.bmp", -2.3f, -0.70f, -4.0f, 2.5, 1.875);
	Tile t8("Data/stork.bmp", 0.7f, -0.86f, -5.5f, 2.0, 1.5);
	Tile t9("Data/fish.bmp",  1.23f, -1.36f, -3.5f, 2.0, 1.5);
	Tile t10("Data/baboon.bmp", -1.67f, 1.75f, -7.5f, 2.0, 1.75);
	Tile t11("Data/wilderbeest.bmp",  0.87f,  1.0f, -5.5f, 2.5, 1.875);

	obj_list.push_back(t0);
	obj_list.push_back(t1);
	obj_list.push_back(t2);
	obj_list.push_back(t3);
	obj_list.push_back(t4);
	obj_list.push_back(t5);
	obj_list.push_back(t6);
	obj_list.push_back(t7);
	obj_list.push_back(t8);
	obj_list.push_back(t9);
	obj_list.push_back(t10);
	obj_list.push_back(t11);


//	GLfloat overlay_color[3] = {1.0f, 0.0f, 0.0f};
//	overlay = overlay_manager.getOverlay("0");

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

	overlay_manager.drawOverlays();
	glutSwapBuffers();

	glutPostRedisplay();
}

void handleUpDown(Overlay * overlay, std::string pointID, GLint button, GLint button_state,
        double normalized_x, double normalized_y) {
	if (cglXStartUpdate()) {
	    if (button_state == GLUT_UP) {
	        state = 0;
	        overlay->removeFinger(pointID);
	        return;
	    }

        if (button == GLUT_LEFT_BUTTON)
            state |= PAN;

        if (button == GLUT_RIGHT_BUTTON)
            state |= ZOOM;

	    overlay->addFinger(pointID, normalized_x, normalized_y, 0.1, 0.1);

	    GLfloat posInOverlay[2];
	    overlay->getPosOfFinger(pointID, posInOverlay);

	    GLint modifiers = glutGetModifiers();

	    obj_iter it, intersected_tile;
	    bool intersected = false;
	    for(it = obj_list.begin(); it != obj_list.end(); it++) {
	        if(it->intersects(posInOverlay[0], posInOverlay[1]) && button_state == GLUT_DOWN) {
	            it->setOverlayID(overlay->getID());
//	            if(modifiers & GLUT_ACTIVE_CTRL)
//	            	it->toggleSelected();
//	            else
	            	it->setSelected(true);

	            intersected_tile = it;
	            intersected = true;
	        } else {
	            if(it->getOverlayID().compare(overlay->getID()) == 0)
//	            if((modifiers & GLUT_ACTIVE_CTRL) == 0)
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

void handleMove(Overlay * overlay, std::string point_id, double normalized_x, double normalized_y) {
	if (cglXStartUpdate()) {
		if (state == 0)
			return;

        GLfloat oldPosition[2];
        overlay->getPosOfFinger(point_id, oldPosition);

        overlay->moveFinger(point_id, normalized_x, normalized_y);

        GLfloat newPosition[2];
        overlay->getPosOfFinger(point_id, newPosition);

        obj_iter it;
        for(it = obj_list.begin(); it != obj_list.end(); it++)
            if(it->isSelected() && it->getOverlayID().compare(overlay->getID()) == 0)
                it->updateTransformations(state, newPosition[0], newPosition[1],
		                                         oldPosition[0], oldPosition[1]);
	}
	cglXUpdateDone();

	glutPostRedisplay();
}

void handleSpecKeys(GLint key, GLint x, GLint y) {
    Overlay * overlay = NULL;
	GLfloat overlay_x = overlay->getX();
	GLfloat overlay_y = overlay->getY();
	GLfloat overlay_w = overlay->getWidth();
	GLfloat overlay_h = overlay->getHeight();

	if (cglXStartUpdate()) {
        GLfloat oldPosition[2];
        overlay->getPosOfFinger("0", oldPosition);

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
        overlay->getPosOfFinger("0", newPosition);

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
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse( msg, root );
        if ( !parsingSuccessful ) {
            printf("Failed to parse configuration\n");
            printf("%s\n", reader.getFormatedErrorMessages());
            return;
        }

        std::string event = root["event"].asString();
        std::string overlay_id = root["overlayID"].asString();
        std::string point_id = root["pointID"].asString();

        Overlay * overlay = overlay_manager.getOverlay(overlay_id);
        GLfloat overlay_w = overlay->getWidth();
        GLfloat overlay_h = overlay->getHeight();

        double normalized_x = 0, normalized_y = 0, scale_factor = 1, absolute = 1;
        if(root["normalizedX"].isDouble())
            normalized_x = root["normalizedX"].asDouble();
        if(root["normalizedY"].isDouble())
            normalized_y = root["normalizedY"].asDouble();
        if(root["absolute"].isDouble())
            absolute = root["absolute"].asDouble();
        if(root["scaleFactor"].isDouble())
            scale_factor += root["scaleFactor"].asDouble();

        if(event.compare("OVERLAY_MOVE") == 0) {
            GLfloat oldPosition[2];
            overlay->getPosOfFinger(point_id, oldPosition);

            GLfloat overlay_x, overlay_y;

            if(absolute == 1) {
                overlay_x = ((normalized_x * getHeadWidth()) - (getHeadWidth() / 2)) / 100.0f;
                overlay_y = -((normalized_y * getHeadHeight()) - (getHeadHeight() / 2)) / 100.0f;
            } else {
                overlay_x = overlay->getX() + normalized_x;
                overlay_y = overlay->getY() + normalized_y;
            }

            overlay_manager.setOverlayPos(overlay_id, overlay_x, overlay_y);

            GLfloat newPosition[2];
            overlay->getPosOfFinger(point_id, newPosition);

            obj_iter it;
            for(it = obj_list.begin(); it != obj_list.end(); it++)
                if(it->isSelected() && it->getOverlayID().compare(overlay->getID()) == 0)
                    it->updateTransformations(state, newPosition[0], newPosition[1],
                                                     oldPosition[0], oldPosition[1]);
        } else if(event.compare("OVERLAY_SCALE") == 0) {
            overlay_manager.setOverlaySize(overlay_id, overlay_w * scale_factor, overlay_h * scale_factor);
        } else if(event.compare("SCALE") == 0) {
	        obj_iter it;
	        for(it = obj_list.begin(); it != obj_list.end(); it++)
	            if(it->isSelected() && it->getOverlayID().compare(overlay->getID()) == 0)
	            	it->setTileSize(it->getWidth() * scale_factor, it->getHeight() * scale_factor);
        } else if(event.compare("MOVE") == 0) {
            handleMove(overlay, point_id, normalized_x, normalized_y);
        } else if(event.compare("DOWN") == 0) {
            handleUpDown(overlay, point_id, GLUT_LEFT_BUTTON, GLUT_DOWN, normalized_x, normalized_y);
        } else if(event.compare("UP") == 0) {
            handleUpDown(overlay, point_id, GLUT_LEFT_BUTTON, GLUT_UP, normalized_x, normalized_y);
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
//	glutMouseFunc(handleMouseClick);
//	glutMotionFunc(handleMouseMove);
//	glutSpecialFunc(handleSpecKeys);
	cglXCustomMsgFunc(handleCustomMsg);
	glutMainLoop();
	return 0; // ANSI C requires main to return int.
}
