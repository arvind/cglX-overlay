/*
 * main.h
 *
 *  Created on: Jul 19, 2010
 *      Author: root
 */

#ifndef MAIN_H_
#define MAIN_H_

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
#include <list>
#include "bmp.h"
#include "overlay.h"
#include "tile.h"

static Overlay * overlay;

static std::list<Tile> obj_list;
typedef std::list<Tile>::iterator obj_iter;
typedef std::list<Tile>::reverse_iterator reverse_obj_iter;

// States //
#define PAN                     1
#define ZOOM                    2
#define ROTATE                  3

static int state;
static int mouse_x, mouse_y;
static Tile * chosen_obj;

void init(void);
void handleReshape(GLint w, GLint h);
void handleDisplay(void);
void handleMouseClick(GLint button, GLint button_state, GLint x, GLint y);
void processMouseClick(bool overlay, GLint button, GLint button_state, GLint x, GLint y);
void handleMouseMove(GLint x, GLint y);
void handleSpecKeys(GLint key, GLint x, GLint y);
void handleXEvent(XEvent * event);
int main(int argc, char** argv);

#endif /* MAIN_H_ */
