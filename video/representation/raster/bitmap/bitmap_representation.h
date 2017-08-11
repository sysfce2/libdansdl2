#ifndef LIBDANSDL2_BITMAP_REPRESENTATION
#define LIBDANSDL2_BITMAP_REPRESENTATION

#include "../raster_representation.h"

namespace ldv
{

//!Representation of a loaded image.

//!Implemented in terms of its parent class almost entirely.

class bitmap_representation:
	public raster_representation
{
	public:

			bitmap_representation();
			bitmap_representation(const texture&, rect={0,0,0,0}, rect={0,0,0,0});
			bitmap_representation(const bitmap_representation&);
			bitmap_representation& operator=(const bitmap_representation &);
};

} //Fin namespace DLibV

#endif
