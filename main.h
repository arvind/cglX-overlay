/*
 * main.h
 *
 *  Created on: Jul 19, 2010
 *      Author: root
 */

#ifndef MAIN_H_
#define MAIN_H_

void init(void);
void handleReshape(GLint w, GLint h);
void handleDisplay(void);
void handleMouseClick(GLint button, GLint button_state, GLint x, GLint y);
void handleMouseMove(GLint x, GLint y);
void handleSpecKeys(GLint key, GLint x, GLint y);
void handleCustomMsg(int len, char *msg);
int main(int argc, char** argv);

#endif /* MAIN_H_ */
