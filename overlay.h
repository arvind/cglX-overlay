#ifndef OVERLAY_H_
#define OVERLAY_H_

#include <utility>
#include <map>
#define PI_2 3.14159*2

class Overlay {
	private:
        class Finger {
            private:
                GLfloat x, y, width, height; // x, y are normalized
                Overlay* overlay;
                void drawOval(GLfloat x_center, GLfloat y_center, GLfloat w, GLfloat h, GLint n);

            public:
                Finger(Overlay* const overlay, GLfloat x, GLfloat y, GLfloat width, GLfloat height);

                GLfloat getX();
                GLfloat getY();
                GLfloat getWidth();
                GLfloat getHeight();

                void drawFinger();
                void getPosInOverlay(GLfloat * posInOverlay);
        };

		GLfloat x, y, z, width, height, color[3];
		std::map<GLint, Overlay::Finger *> fingers;
		void drawOverlayShape(void);

	public:
		Overlay(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat * color);
		typedef std::map<GLint, Overlay::Finger *>::iterator finger_iter;

		GLfloat getX();
		GLfloat getY();
		GLfloat getZ();
		GLfloat getWidth();
		GLfloat getHeight();

		void setOverlay(GLfloat new_x, GLfloat new_y, GLfloat new_w, GLfloat new_h);
		void setOverlaySize(GLfloat new_w, GLfloat new_h);
		void setOverlayPos(GLfloat new_x, GLfloat new_y);
		void drawOverlay(void);
		void getBoundingBox(GLfloat * bounding_box);

		void addFinger(GLint id, GLfloat x, GLfloat y, GLfloat width, GLfloat height);
		void removeFinger(GLint id);
		void moveFinger(GLint id, GLfloat x, GLfloat y);
		void getPosOfFinger(GLint id, GLfloat * posInOverlay);
};

#endif /* OVERLAY_H_ */


