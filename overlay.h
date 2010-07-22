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

class Overlay {
	private:
		GLfloat x, y, z, width, height;
		void drawOverlayShape(void);

	public:
		Overlay();
		Overlay(GLfloat z);
		Overlay(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h);

		GLfloat getX();
		GLfloat getY();
		GLfloat getZ();
		GLfloat getWidth();
		GLfloat getHeight();

		void setOverlay(GLfloat new_x, GLfloat new_y, GLfloat new_w, GLfloat new_h);
		void setOverlaySize(GLfloat new_w, GLfloat new_h);
		void setOverlayPos(GLfloat new_x, GLfloat new_y);
		void drawOverlay(void);
		GLfloat * getPosInOverlay(GLfloat normalized_x, GLfloat normalized_y);
		GLfloat * getBoundingBox(void);
};




