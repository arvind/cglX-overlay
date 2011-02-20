/*
 * overlay_manager.cpp
 *
 *  Created on: Jan 22, 2011
 *      Author: Arvind Satyanarayan
 */

#ifdef DEF_USE_CGLX
#ifdef __APPLE__
    #include <cglX/cglX.h>
#else
    #include "cglX.h"
#endif
    using namespace std;
#else
    #include <GL/glut.h>
#endif
#include "overlay_manager.h";

OverlayManager::OverlayManager() {
    GLfloat color[3];
    this->getOverlayColor(color);

//    Overlay * default_overlay = new Overlay(0.0f, 0.0f, -6.0f, 2.0f, 1.4f, color);
//    this->overlays.insert(pair<string, Overlay *>("0", default_overlay));
}

void OverlayManager::getOverlayColor(GLfloat * color) {
    GLfloat overlay_colors[3][3] = {{1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}};

    GLint which_color = this->overlays.size() % 3;
    color[0] = overlay_colors[which_color][0];
    color[1] = overlay_colors[which_color][1];
    color[2] = overlay_colors[which_color][2];
}

Overlay * OverlayManager::addOverlay(string overlay_id) {
    GLfloat color[3];
    this->getOverlayColor(color);

    Overlay * default_overlay = new Overlay(overlay_id, 0.0f, 0.0f, -6.0f, 2.0f, 1.4f, color);
    this->overlays.insert(pair<string, Overlay *>(overlay_id, default_overlay));

    return default_overlay;
}

//Overlay * OverlayManager::addOverlay(Overlay * overlay) {
//    char overlay_id[10];
//    string overlay_strid(overlay_id);
//    this->overlays.insert(pair<string, Overlay *>(overlay_strid, overlay));
//
//    return overlay;
//}

map<string, Overlay*> OverlayManager::getOverlays() {
    return this->overlays;
}

Overlay * OverlayManager::getOverlay(string overlay_id) {
    if(this->overlays.size() > 0) {
        overlay_iter it = this->overlays.find(overlay_id);
        if(it != this->overlays.end())
            return it->second;
    }

    return this->addOverlay(overlay_id);
}

void OverlayManager::removeOverlay(string overlay_id) {
    if(this->overlays.size() <= 1)
        return;

    overlay_iter it = this->overlays.find(overlay_id);
    if(it->first == overlay_id)
        this->overlays.erase(it);
}

void OverlayManager::setOverlaySize(string overlay_id, GLfloat new_w, GLfloat new_h) {
    Overlay * overlay = this->getOverlay(overlay_id);
    overlay->setOverlaySize(new_w, new_h);
    this->checkSpheresOfInfluence(overlay);
}

void OverlayManager::setOverlayPos(string overlay_id, GLfloat new_x, GLfloat new_y) {
    Overlay * overlay = this->getOverlay(overlay_id);
    overlay->setOverlayPos(new_x, new_y);
    this->checkSpheresOfInfluence(overlay);
}

void OverlayManager::drawOverlays() {
    for(overlay_iter it = this->overlays.begin(); it != this->overlays.end(); it++) {
        it->second->drawOverlay();
    }
}

void OverlayManager::checkSpheresOfInfluence(Overlay * overlay) {
    GLfloat boundingBoxWithSOI[4];
    overlay->getBoundingBoxWithSOI(boundingBoxWithSOI);

    for(overlay_iter it = this->overlays.begin(); it != this->overlays.end(); it++) {
        Overlay * o = (Overlay *) it->second;

        GLfloat intersectBox[4];
        o->getBoundingBoxWithSOI(intersectBox);

        // First check intersection - to intersect they must intersect on both x and y axes
        if(((boundingBoxWithSOI[0] > intersectBox[0] && boundingBoxWithSOI[0] < intersectBox[2]) ||
            (boundingBoxWithSOI[2] > intersectBox[0] && boundingBoxWithSOI[2] < intersectBox[2])) &&
           ((boundingBoxWithSOI[1] > intersectBox[1] && boundingBoxWithSOI[1] < intersectBox[3]) ||
            (boundingBoxWithSOI[3] > intersectBox[1] && boundingBoxWithSOI[3] < intersectBox[3]))) {

            GLfloat intersecting_values[4];
            intersecting_values[0] = intersectBox[2] - boundingBoxWithSOI[0]; // iMaxX - minX
            intersecting_values[1] = boundingBoxWithSOI[2] - intersectBox[0]; // maxX - iMinX
            intersecting_values[2] = intersectBox[3] - boundingBoxWithSOI[1]; // iMaxY - minY
            intersecting_values[3] = boundingBoxWithSOI[3] - intersectBox[1]; // maxY - iMinY

            // The direction to move o will be based on which of the four sides the smallest
            // intersecting area (> 0) is. If the minimum intersecting area is:
            // [0] - intersection between overlay's left edge and o's right edge, move o left
            // [1] - intersection between overlay's right edge and o's left edge, move o right
            // [2] - intersection between overlay's bottom edge and o's top edge, move o down
            // [3] - intersection between overlay's top edge and o's bottom edge, move o up
            int min_index = 0;
            GLfloat min_intersection = intersecting_values[0];

            for(int i = 0; i < 3; i++)
                if(intersecting_values[i] > 0 && intersecting_values[i] < min_intersection)
                    min_index = i;

            switch(min_index) {
                case 0:
                    this->setOverlayPos(o->getID(), o->getX() - intersecting_values[0], o->getY());
                break;

                case 1:
                    this->setOverlayPos(o->getID(), o->getX() + intersecting_values[1], o->getY());
                break;

                case 2:
                    this->setOverlayPos(o->getID(), o->getX(), o->getY() - intersecting_values[2]);
                break;

                case 3:
                    this->setOverlayPos(o->getID(), o->getX(), o->getY() + intersecting_values[3]);
                break;
            }
        }
    }
}
