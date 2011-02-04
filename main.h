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
//void handleMouseClick(GLint button, GLint button_state, GLint x, GLint y);
void handleUpDown(Overlay * overlay, std::string pointID, GLint button, GLint button_state,
        double normalized_x, double normalized_y);
//void handleMouseMove(GLint x, GLint y);
void handleMove(Overlay * overlay, std::string point_id, double normalized_x, double normalized_y);
void handleSpecKeys(GLint key, GLint x, GLint y);
void handleCustomMsg(int len, char *msg);
int main(int argc, char** argv);

#endif /* MAIN_H_ */
