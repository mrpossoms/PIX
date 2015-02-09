#ifndef PIX_RENDERER
#define PIX_RENDERER

#include <unistd.h>

// return codes
#define PIX_ERROR_WINDOW_FAILED_TO_OPEN 0x00000001

// renderer setup settings
#define PIX_RENDER_SURFACE_COLOR 0x00000001
#define PIX_RENDER_SURFACE_DEPTH 0x00000002


struct PixDesc{
	unsigned int width, height;
	unsigned int flags;
	const char*  name;
};

int  pixInit(struct PixDesc settings);
void pixDestroy();

int  pixShouldBegin();
void pixPresent();

void pixClear();

#endif
