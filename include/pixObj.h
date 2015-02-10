#ifndef PIX_OBJECT
#define PIX_OBJECT

#include "pixGeo.h"
#include "pixShader.h"

struct PixObj{
	struct PixGeo*    geo;
	struct PixShader* shader;
	void*             renderParams;

};

#endif