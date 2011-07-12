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
#include <cmath>;

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

    Overlay * default_overlay = new Overlay(overlay_id, 0.0f, 0.0f, -6.0f, 1.5f, 1.05f, color);
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
    if(it != this->overlays.end())
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
    GLfloat overlayBound[4];
    overlay->getBoundingBoxWithSOI(overlayBound);

    const GLfloat display_zone = 0.3f;

    for(overlay_iter it = this->overlays.begin(); it != this->overlays.end(); it++) {
        Overlay * otherlay = (Overlay *) it->second;

        GLfloat otherlayBound[4];
        otherlay->getBoundingBoxWithSOI(otherlayBound);

        if(otherlay->getID() == overlay->getID())
            continue;

        if(otherlay->getFingers().size() > 0)    // If someone's working in their overlay,
            continue;                     // don't move it. That's just rude!

        GLfloat intersecting_values[4];
        intersecting_values[0] = otherlayBound[2] - overlayBound[0]; // iMaxX - minX
        intersecting_values[1] = overlayBound[2] - otherlayBound[0]; // maxX - iMinX
        intersecting_values[2] = otherlayBound[0] - overlayBound[2]; // iMinX - maxX
        intersecting_values[3] = overlayBound[0] - otherlayBound[2]; // minX - iMaxX

        intersecting_values[4] = otherlayBound[3] - overlayBound[1]; // iMaxY - minY
        intersecting_values[5] = overlayBound[3] - otherlayBound[1]; // maxY - iMinY
        intersecting_values[6] = otherlayBound[1] - overlayBound[3]; // iMinY - maxY
        intersecting_values[7] = overlayBound[1] - otherlayBound[3]; // minY - iMaxY


        // If two overlays are "close", show their spheres
        if((abs(intersecting_values[0]) <= display_zone || abs(intersecting_values[1]) <= display_zone ||
            abs(intersecting_values[2]) <= display_zone || abs(intersecting_values[3]) <= display_zone) &&
           (abs(intersecting_values[4]) <= display_zone || abs(intersecting_values[5]) <= display_zone ||
            abs(intersecting_values[6]) <= display_zone || abs(intersecting_values[7]) <= display_zone)) {
            otherlay->setDrawSphere(true);
            overlay->setDrawSphere(true);
        } else {
            otherlay->setDrawSphere(false);
            overlay->setDrawSphere(false);
        }

        // First check intersection - to intersect they must intersect on both x and y axes
        if(((overlayBound[0] > otherlayBound[0] && overlayBound[0] < otherlayBound[2]) ||
            (overlayBound[2] > otherlayBound[0] && overlayBound[2] < otherlayBound[2])) &&
           ((overlayBound[1] > otherlayBound[1] && overlayBound[1] < otherlayBound[3]) ||
            (overlayBound[3] > otherlayBound[1] && overlayBound[3] < otherlayBound[3]))) {

            // The direction to move o will be based on which of the four sides the smallest
            // intersecting area (> 0) is. We check the smallest intersecting area as this will
            // give us an indication of how overlay is moving in relation to o.
            // If the minimum intersecting area is along the x axis, move o along the y axis and vice-versa.
            int min_index = 0;
            GLfloat min_intersection = intersecting_values[0];

            for(int i = 0; i < 4; i++) {
                if(intersecting_values[i] > 0 && intersecting_values[i] < min_intersection) {
                    min_index = i;
                    min_intersection = intersecting_values[i];
                }
            }

            if(min_index == 0 || min_index == 1) {  // Smallest intersection along x-axis
                GLfloat new_y = otherlay->getY();
                if(otherlay->getY() < overlay->getY()) {
                    new_y = overlayBound[1] - (2 * otherlay->getSphereSize()) - (otherlay->getHeight() / 2);
                } else if(otherlay->getY() > overlay->getY()) {
                    new_y = overlayBound[3] + (2 * otherlay->getSphereSize()) + (otherlay->getHeight() / 2);
                }

                this->setOverlayPos(otherlay->getID(), otherlay->getX(), new_y);
            } else {                                // Smallest intersection along y-axis
                GLfloat new_x = otherlay->getY();
                if(otherlay->getX() < overlay->getX()) {
                    new_x = overlayBound[0] - (2 * otherlay->getSphereSize()) - (otherlay->getWidth() / 2);
                } else if(otherlay->getX() > overlay->getX()) {
                    new_x = overlayBound[2] + (2 * otherlay->getSphereSize()) + (otherlay->getWidth() / 2);
                }

                this->setOverlayPos(otherlay->getID(), new_x, otherlay->getY());
            }
        }
    }
}
