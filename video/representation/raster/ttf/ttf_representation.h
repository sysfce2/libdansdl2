#ifndef LIBDANSDL2_TTF_REPRESENTATION_H
#define LIBDANSDL2_TTF_REPRESENTATION_H

#include <memory>
#include "../raster_representation.h"
#include "../../../ttf_font/ttf_font.h"
#include "../../../texture/texture.h"
#include "../../../surface/surface.h"
#include "../../../surface/canvas.h"
#include "../../../surface/image.h"
#include "../../../../tools/log/log.h"
#include "../../../../tools/tools/tools.h"

namespace ldv
{

//!Text representation using a TTF font.

//!Generates and owns a texture using the SDL_ttf library. Textures generated 
//!are always in powers of two, to avoid hardware issues. Also, note that 
//!depending on the font used, the size will not always correspond to a certain
//!pixel height.

class ttf_representation:
	public raster_representation
{
	public:

	enum class 			render_mode{solid, shaded, blended}; 	//!< Render modes. Default is blended.
	enum class			text_align{left, center, right};	//!< Text alignment Default is left.

					ttf_representation(const ttf_font&, rgba_color, std::string="", double=1., text_align=text_align::left, render_mode=render_mode::blended);
					ttf_representation(const ttf_representation&);
	virtual				~ttf_representation();
	ttf_representation&		operator=(const ttf_representation&);

	//!Returns the assigned text.
	const std::string& 		get_text() const {return text;}

	void				set_color(rgb_color);
	void				set_bg_shaded_color(rgba_color);

	void				set_text_align(text_align);

	void 				set_font(const ttf_font&);
	void 				set_text(const char);
	void 				set_text(const std::string&);
	void				set_render_mode(render_mode r);
	void				set_line_height_ratio(double);

	private:
	
	void				create_texture();
	void				set_text_internal(const std::string&);
	void				text_replace(std::string&, const std::string&, const std::string&);

	ttf_font const *		font; //! <This is a pointer so it can change.
	std::string			text;

	render_mode			mode;
	rgb_color			text_color;
	rgba_color			bg_shaded;
	double				line_height_ratio; //!< Expressed as a ratio of the font size.
	text_align			alignment;

	//!Lookup table for powers of two.
	static const std::vector<int>	valid_sizes;
};

}

#endif
