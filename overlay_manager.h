/*
 * overlay_manager.h
 *
 *  Created on: Jan 20, 2011
 *      Author: Arvind Satyanarayan <arvind@ucsd.edu>
 */

#ifndef OVERLAY_MANAGER_H_
#define OVERLAY_MANAGER_H_

#include "overlay.h"
#include <map>
using namespace std;

class OverlayManager {
    private:
        map<string, Overlay *> overlays;
        void getOverlayColor(GLfloat * color);

    public:
        typedef map<std::string, Overlay *>::iterator overlay_iter;

        OverlayManager();
        void addOverlay(std::string overlay_id);
        void addOverlay(Overlay * overlay);
        map<std::string, Overlay*> getOverlays();
        Overlay * getOverlay(std::string overlay_id);
        void removeOverlay(std::string overlay_id);
        void setOverlaySize(std::string overlay_id, GLfloat new_w, GLfloat new_h);
        void setOverlayPos(std::string overlay_id, GLfloat new_x, GLfloat new_y);
        void drawOverlays();
};

#endif /* OVERLAY_MANAGER_H_ */
