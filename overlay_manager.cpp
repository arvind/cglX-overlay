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
    using namespace cglx;
#else
    #include <GL/glut.h>
#endif
#include "overlay_manager.h";
#include <stdio.h>
#include <stdlib.h>

OverlayManager::OverlayManager() {
    GLint overlay_color = this->overlays.size() % 3;
    Overlay * default_overlay = new Overlay(0.0f, 0.0f, -6.0f, 2.0f, 1.4f, this->overlay_color[overlay_color]);
    this->overlays.insert(std::pair<std::string, Overlay *>("0", default_overlay));
}

void OverlayManager::addOverlay(std::string overlay_id) {
    GLint overlay_color = this->overlays.size() % 3;
    Overlay * default_overlay = new Overlay(0.0f, 0.0f, -6.0f, 2.0f, 1.4f, this->overlay_color[overlay_color]);
    this->overlays.insert(std::pair<std::string, Overlay *>(overlay_id, default_overlay));
}

void OverlayManager::addOverlay(Overlay * overlay) {
    char overlay_id[10];
    sprintf(overlay_id, "%d", this->overlays.size());
    std::string overlay_strid(overlay_id);
    this->overlays.insert(std::pair<std::string, Overlay *>(overlay_strid, overlay));
}

std::map<std::string, Overlay*> OverlayManager::getOverlays() {
    return this->overlays;
}

Overlay * OverlayManager::getOverlay(std::string overlay_id) {
    overlay_iter it = this->overlays.find(overlay_id);
    return it->second;
}

void OverlayManager::removeOverlay(std::string overlay_id) {
    if(this->overlays.size() <= 1)
        return;

    overlay_iter it = this->overlays.find(overlay_id);
    if(it->first == overlay_id)
        this->overlays.erase(it);
}

void OverlayManager::setOverlaySize(std::string overlay_id, GLfloat new_w, GLfloat new_h) {
    Overlay * overlay = this->getOverlay(overlay_id);
    overlay->setOverlaySize(new_w, new_h);
}

void OverlayManager::setOverlayPos(std::string overlay_id, GLfloat new_x, GLfloat new_y) {
    Overlay * overlay = this->getOverlay(overlay_id);
    overlay->setOverlayPos(new_x, new_y);
}

