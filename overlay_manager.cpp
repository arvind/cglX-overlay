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

    Overlay * default_overlay = new Overlay(0.0f, 0.0f, -6.0f, 2.0f, 1.4f, color);
    this->overlays.insert(pair<string, Overlay *>("0", default_overlay));
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

void OverlayManager::addOverlay(string overlay_id) {
    GLfloat color[3];
    this->getOverlayColor(color);

    Overlay * default_overlay = new Overlay(0.0f, 0.0f, -6.0f, 2.0f, 1.4f, color);
    this->overlays.insert(pair<string, Overlay *>(overlay_id, default_overlay));
}

void OverlayManager::addOverlay(Overlay * overlay) {
    char overlay_id[10];
    sprintf(overlay_id, "%d", this->overlays.size());
    string overlay_strid(overlay_id);
    this->overlays.insert(pair<string, Overlay *>(overlay_strid, overlay));
}

map<string, Overlay*> OverlayManager::getOverlays() {
    return this->overlays;
}

Overlay * OverlayManager::getOverlay(string overlay_id) {
    overlay_iter it = this->overlays.find(overlay_id);
    return it->second;
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
}

void OverlayManager::setOverlayPos(string overlay_id, GLfloat new_x, GLfloat new_y) {
    Overlay * overlay = this->getOverlay(overlay_id);
    overlay->setOverlayPos(new_x, new_y);
}

void OverlayManager::drawOverlays() {
    for(overlay_iter it = this->overlays.begin(); it != this->overlays.end(); it++) {
        it->second->drawOverlay();
    }
}
