#include <ldv/ttf_representation.h>
#include <sstream>

#ifdef LIBDANSDL2_DEBUG
#include <ldt/log.h>
#endif

using namespace ldv;

const std::vector<int> ttf_representation::valid_sizes={2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

//!Default class constructor.

//!Line height by default is constructed as font size values.

ttf_representation::ttf_representation(const ttf_font& pfont, rgba_color pcolor, std::string ptext, double lhr, text_align al, render_mode pmode)
	:raster_representation(rect{0,0,0,0}, rect{0,0,0,0}, colorif(pcolor.a)),
	font(&pfont),
	text(ptext),
	mode(pmode),
	text_color{pcolor.r, pcolor.g, pcolor.b},
	bg_shaded(rgba8(0,0,0,255)),
	line_height_ratio(lhr),
	alignment(al),
	text_position{0,0,0,0},
	text_x_displacement{0} {

	create_texture();
}

//!Copy constructor.

//!Texture is recreated as a different resource from the original.

ttf_representation::ttf_representation(const ttf_representation& o)
	:raster_representation(o),
	font(o.font),
	text(o.text),
	mode(o.mode),
	text_color(o.text_color),
	bg_shaded(o.bg_shaded),
	text_position{o.text_position},
	text_x_displacement{o.text_x_displacement} {

	reset_texture();
	create_texture();
}

//!Class destructor.

ttf_representation::~ttf_representation() {

	free_texture();
}

ttf_representation& ttf_representation::operator=(const ttf_representation& o) {

	raster_representation::operator=(o);
	font=o.font;
	text=o.text;
	mode=o.mode;
	text_color=o.text_color;
	bg_shaded=o.bg_shaded;
	text_position=o.text_position;
	text_x_displacement=o.text_x_displacement;
	create_texture();

	return *this;
}

void ttf_representation::create_texture() {

	if(-1==max_width) {

		create_texture_free_size();
	}
	else {

		create_texture_fixed_width();
	}
}

//!Creates the texture from text.

//!This internal function is called whenever a change on text or font is
//!detected. It changes the clip and position of the representation.
//!It may throw a std::runtime_error if the representation cannot be prepared
//!, which happens when the TTF_* functions fail.
//!Subsequent calls reuse the same texture data.
//!A change of color will not trigger a new texture.
//!This could be considered a bug. There's also another bug where rgb must be
//!converted to bgr...

//TODO: Check RGB and BGR and endianess and such.

void ttf_representation::create_texture_free_size() {

#ifdef LIBDANSDL2_DEBUG
	if(debug_flags & dflag_width_mode) {

		lm::log(ldt::log_lsdl::get()).debug()<<"ldv::ttf_representation::create_texture_free_size"<<std::endl;
	}
#endif

	//The text is prepared line by line in different surfaces.
#ifdef WINBUILD
	auto lines=explode(text, "\r\n");
#else
	auto lines=explode(text, "\n");
#endif
	
	//Measuring the full resulting texture...
	int h=0, w=0, total_w=0;
	for(std::string& c : lines) {

		text_replace(c, "\t", "    ");

		TTF_SizeUTF8(const_cast<TTF_Font*>(font->get_font()), c.c_str(), &w, &h);
		if(w > total_w) {

			total_w=w;
		}
	}

#ifdef LIBDANSDL2_DEBUG
	if(debug_flags & dflag_width_mode) {

		lm::log(ldt::log_lsdl::get()).debug()<<"ldv::ttf_representation::create_texture_free_size will result on a text of "<<total_w<<"x"<<h<<" with "<<lines.size()<<"lines"<<std::endl;
	}
#endif

	create_texture_internal(total_w, h, lines);
}

void ttf_representation::create_texture_fixed_width() {

#ifdef LIBDANSDL2_DEBUG
	if(debug_flags & dflag_width_mode) {

		lm::log(ldt::log_lsdl::get()).debug()<<"ldv::ttf_representation::create_texture_fixed_width"<<std::endl;
	}
#endif
	//The text is prepared line by line in different surfaces.
	
#ifdef WINBUILD
	auto original_lines=explode(text, "\r\n");
#else
	auto original_lines=explode(text, "\n");
#endif

//TODO: Add debug stuff to this so at least I can't know what's going on here???
	
	std::vector<std::string> lines;

	//Measuring the full resulting texture...
	int h=0, w=0, total_w=0;
	for(auto& line : original_lines) {

		text_replace(line, "\t", "    ");
		std::string word{}, curline{};
		std::size_t consecutive_spaces=0;
		std::stringstream ss{line};
		bool done=false;

		auto add_line=[&total_w, &lines](const std::string& _str, int _size) {

			if(_size > total_w) {

				total_w=_size;
			}

			lines.push_back(_str);
		};

		while(true) {

			char c=ss.get();
			if(ss.eof()) {
				c=' '; //Add a space to trigger the end of the word.
				done=true;
			}

			if(isspace(c)) {

				if(!word.size()) {

					++consecutive_spaces;
				}
				else {

					//End of the word... put the space back, pls...
					if(!done) {

						ss.unget();
					}

					std::string spacing=consecutive_spaces
						? std::string(consecutive_spaces, ' ')
						: std::string{};

					TTF_SizeUTF8(const_cast<TTF_Font*>(font->get_font()), (spacing+word).c_str(), &w, &h);
					int wordsize_with_spacing=w;

					TTF_SizeUTF8(const_cast<TTF_Font*>(font->get_font()), curline.c_str(), &w, &h);
					int curlinesize=w;

					//There's place for something more...
					if(wordsize_with_spacing+curlinesize <= max_width) {

						curline+=spacing+word;
						if(done) { //Last word was reached and there was still place...

							add_line(curline, wordsize_with_spacing+curlinesize);
							curline.clear();
						}
					}
					//can't fit it...
					else {

						//there were words... spacing does not count.
						if(curline.size()) {

							add_line(curline, curlinesize);
							curline=word; //stray word is set as the beginning of the next line.
						}
						//there were no words yet, spacing counts...
						else {

							add_line(spacing+word, wordsize_with_spacing);
						}
					}

					//Clear counters.
					word.clear();
					consecutive_spaces=0;
				}

				if(done) {

					TTF_SizeUTF8(const_cast<TTF_Font*>(font->get_font()), curline.c_str(), &w, &h);
					add_line(curline, w);
					break;
				}
			}
			else {
				word+=c;
			}
		}
	}

#ifdef LIBDANSDL2_DEBUG
	if(debug_flags & dflag_width_mode) {

		lm::log(ldt::log_lsdl::get()).debug()<<"ldv::ttf_representation::create_texture_fixed_width will result on a text of "<<total_w<<"x"<<h<<" with "<<lines.size()<<"lines"<<std::endl;
	}
#endif

	create_texture_internal(total_w, h, lines);
}

void ttf_representation::create_texture_internal(

	int _total_w,
	int _h,
	const std::vector<std::string>& _lines) {

	int line_height=(double)font->get_size()*line_height_ratio;
	int total_h=(_lines.size() * _h) + ( (_lines.size()-1) * (line_height-_h)); //The last spacing is removed, of course.

	auto canvas_w=get_next_power_of_two(_total_w);
	auto canvas_h=get_next_power_of_two(total_h);

#ifdef LIBDANSDL2_DEBUG
	if(debug_flags & dflag_internal_size) {

		lm::log(ldt::log_lsdl::get()).debug()<<"ldv::ttf_representation::create_texture_internal, canvas size is "<<canvas_w<<"x"<<canvas_h<<std::endl;
	}
#endif

	//A raw surface will be created with a given color, so we can extract its internal flags.
	//This is going to render a surface. Alpha will be always 1. When rendering it will be applied and colorised.
	//Also, notice the hack. The shaded thing will create a BGRA, so we change the colors.

	SDL_Color sdl_col{(Uint8)colorif(text_color.r), (Uint8)colorif(text_color.g), (Uint8)colorif(text_color.b), (Uint8)colorif(1.f)};
	if(mode==render_mode::blended) {

		sdl_col=SDL_Color{(Uint8)colorif(text_color.r), (Uint8)colorif(text_color.g), (Uint8)colorif(text_color.b), (Uint8)colorif(1.f)};
	}


	//We are attempting to create a surface here to later copy its format. So ugly...
	//TODO: This is so ugly.
	//TODO: raw pointers!
	SDL_Surface * s=nullptr;
	switch(mode) {
		case render_mode::solid:
			s=TTF_RenderUTF8_Solid(const_cast<TTF_Font*>(font->get_font()), "a", sdl_col);
		break;
		case render_mode::shaded:
			s=TTF_RenderUTF8_Shaded(const_cast<TTF_Font*>(font->get_font()), "a", sdl_col,
				SDL_Color{(Uint8)colorif(bg_shaded.r), (Uint8)colorif(bg_shaded.g), (Uint8)colorif(bg_shaded.b), (Uint8)colorif(bg_shaded.a)});
		break;
		case render_mode::blended: {
			//Produces a ARGB surface, we convert it to RGBA.
			SDL_Surface * argb=TTF_RenderUTF8_Blended(const_cast<TTF_Font*>(font->get_font()), "a", sdl_col);
			s=SDL_ConvertSurfaceFormat(argb, SDL_PIXELFORMAT_RGBA32, 0);
			SDL_FreeSurface(argb);
		//	s=TTF_RenderUTF8_Blended(const_cast<TTF_Font*>(font->get_font()), "a", sdl_col);
		}
		break;
	}

	std::unique_ptr<canvas> cnv(canvas::create(
		canvas_w,
		canvas_h,
		s->format->BitsPerPixel,
		s->format->Rmask,
		s->format->Gmask,
		s->format->Bmask,
		s->format->Amask
	));

	SDL_FreeSurface(s);

	//Creating surfaces for each lines, pasting them on the canvas...
	int y=0;
	int text_x=_total_w; //A suitably large value...

	for(const std::string& c : _lines) {

		SDL_Surface * surf=nullptr;
		const char * cad=c.size() ? c.c_str() : " \0";

		switch(mode) {
			case render_mode::solid:
				surf=TTF_RenderUTF8_Solid(const_cast<TTF_Font*>(font->get_font()), cad, sdl_col);
			break;
			case render_mode::shaded:
				surf=TTF_RenderUTF8_Shaded(const_cast<TTF_Font*>(font->get_font()), cad, sdl_col,
					SDL_Color{(Uint8)colorif(bg_shaded.r), (Uint8)colorif(bg_shaded.g), (Uint8)colorif(bg_shaded.b), (Uint8)colorif(bg_shaded.a)});
			break;
			case render_mode::blended: {
				//Produces a ARGB surface, 
				SDL_Surface * argb=TTF_RenderUTF8_Blended(const_cast<TTF_Font*>(font->get_font()), cad, sdl_col);
				surf=SDL_ConvertSurfaceFormat(argb, SDL_PIXELFORMAT_RGBA32, 0);
				SDL_FreeSurface(argb);
				//surf=TTF_RenderUTF8_Blended(const_cast<TTF_Font*>(font->get_font()), cad, sdl_col);
			}
			break;
		}

		if(nullptr==surf) {

			throw std::runtime_error("Unable to prepare ttf_representation : "+std::string(TTF_GetError())+" : "+c);
		}

		//TODO: We should not do this again, it has been done earlier.
		int w=0, h=0;
		TTF_SizeUTF8(const_cast<TTF_Font*>(font->get_font()), cad, &w, &h);

		int x=0;

		switch(alignment) {
			case text_align::left: x=0; break;
			case text_align::center: x=(canvas_w/2)-(w/2); break;
			case text_align::right: x=canvas_w-w; break;
		}

		text_x=x < text_x ? x : text_x;

		SDL_Rect pos{x, y, 0, 0};
		SDL_BlitSurface(surf, nullptr, cnv->get_surface(), &pos);
		SDL_FreeSurface(surf);
		y+=h+(line_height-h);
	}

/**
	//Multiplatform endianess is always ready to break stuff up.
	SDL_Surface* converted_raw=SDL_ConvertSurfaceFormat(cnv->get_surface(), SDL_PIXELFORMAT_RGBA32, 0);
	surface converted(converted_raw);
*/

	if(!get_texture())	{
		ldv::texture * tex=new ldv::texture(*cnv);
		set_texture(*tex);
	}
	else {
		get_texture()->replace(*cnv);
	}

	set_blend(representation::blends::alpha);
	set_clip({0,0, (unsigned)canvas_w, (unsigned)canvas_h});
	//This must be triggered: dimensions would be left at 0 and cameras would fail.
	set_location({0, 0, (unsigned)canvas_w, (unsigned)canvas_h});

	text_x_displacement=text_x;
	text_position.origin.x=text_x_displacement;
	text_position.origin.y=0;
	text_position.w=(unsigned)_total_w;
	text_position.h=(unsigned)total_h;

	base_view_position.origin=text_position.origin;
	base_view_position.w=canvas_w;
	base_view_position.h=canvas_h;
}

//!Sets a new ttf font.

//!Triggers a cleanse and recreation of the texture if unlocked.

void ttf_representation::set_font(const ttf_font& f) {

	font=&f;
	if(!perform_changes) {
		return;
	}
	create_texture();
}

//!Sets the text as a single character.

//!Triggers a cleanse and recreation of the texture if unlocked.

void ttf_representation::set_text(const char c) {

	//This is absurd...
	std::string temp("");
	temp+=c;
	set_text_internal(temp);
}

//!Internal function to change text.

//!Triggers a change of the text property and the creation of a new texture if unlocked.

void ttf_representation::set_text_internal(const std::string& c) {

	if(c!=text) {
		//The texture is going to be reused.
		text=c;
		if(!perform_changes) {
			return;
		}
		auto pos=get_position();
		reset_calculations();
		create_texture();
		go_to(pos); //Setting the text incidentally resets the position.
	}
}

//!Unlocks the representation and refreshes its values.
void ttf_representation::unlock_changes() {

	perform_changes=true;
	auto pos=get_position();
	reset_calculations();
	create_texture();
	go_to(pos);
}

//!Internal replacement function.

//!Basically keeps this class independent from the "tools" project.

void ttf_representation::text_replace(
	std::string& sujeto, 
	const std::string& busca, 
	const std::string& reemplaza
) {

	size_t pos = 0, l=reemplaza.length();
	while ((pos = sujeto.find(busca, pos)) != std::string::npos) {

		sujeto.replace(pos, busca.length(), reemplaza);
		pos += l;
	}
}

//!Sets the line height as a ratio of the font size.

//!A value of 1.0 given an space equal to the font size. 1.5 gives a line
//!and a half and so on.
//!This function will trigger a recreation of the texture if unlocked.

void ttf_representation::set_line_height_ratio(double v) {

	if(v==line_height_ratio) {
		return;
	}

	line_height_ratio=v;
	if(!perform_changes) {
		return;
	}

	create_texture();
}

//!Sets the font color.

//!Will trigger a recreation of the texture if different and unlocked.
void ttf_representation::set_color(rgb_color c) {

	if(c==text_color) {
		return;
	}

	text_color=c;
	if(!perform_changes) {
		return;
	}

	auto pos=get_position();
	reset_calculations();
	create_texture();
	go_to(pos); //Setting the color incidentally resets the position.
}

//!Will trigger a recreation of the texture if different and unlocked.
void ttf_representation::set_color(rgba_color _c) {

	auto c=ldv::rgb_color(_c.r, _c.g, _c.b);

	if(c==text_color && get_alphaf()==_c.a) {
		return;
	}

	text_color=c;
	raster_representation::set_alphaf(_c.a);
	if(!perform_changes) {
		return;
	}

	auto pos=get_position();
	reset_calculations();
	create_texture();
	go_to(pos); //Setting the color incidentally resets the position.
}


//!Sets the background color for the shaded mode.

//!Will trigger a recreation of the texture if the mode is shaded and changes (if unlocked).

void ttf_representation::set_bg_shaded_color(rgba_color c) {

	if(c==bg_shaded) return;
	bg_shaded=c;
	if(mode!=render_mode::shaded) return;
	if(!perform_changes) return;
	create_texture();
}

//!Sets the render mode.

//!Triggers a texture recreation if changes and unlocked.

void ttf_representation::set_render_mode(render_mode r) {

	if(r==mode) return;
	mode=r;
	if(!perform_changes) return;
	create_texture();
}

//!Sets the text alignment.

//!Triggers a texture recreation if changes and unlocked. Text alignment only
//!matters when there is more than a single line in the text.

void ttf_representation::set_text_align(text_align v) {

	if(v==alignment) return;
	alignment=v;
	if(!perform_changes) return;

	auto pos=get_position();
	reset_calculations();
	create_texture();
	go_to(pos); //Setting the text alignment incidentally resets the position.
}

void ttf_representation::do_draw() {

	if(!perform_changes) {
		throw std::runtime_error("ttf_representation is locked!");
	}

	raster_representation::do_draw();
}

void ttf_representation::go_to(point _p) {

	base_view_position.origin=_p;
	text_position.origin=_p;
	_p.x-=text_x_displacement;
	raster_representation::go_to(_p);
}

ttf_representation&	ttf_representation::reset_style() {

	TTF_Font * fontptr=const_cast<TTF_Font*>(font->get_font());
	if(nullptr==fontptr) {
		throw std::runtime_error("cannot reset style on null font");
	}

	if(TTF_STYLE_NORMAL != TTF_GetFontStyle(fontptr)) {

		TTF_SetFontStyle(fontptr, TTF_STYLE_NORMAL);
	}

	return *this;
}

ttf_representation&	ttf_representation::set_style(int _flags) {

	TTF_Font * fontptr=const_cast<TTF_Font*>(font->get_font());
	if(nullptr==fontptr) {
		throw std::runtime_error("cannot set style on null font");
	}

	//TODO: Please, check it.
	//This is supposed to leave out invalid flags.
	int flags=_flags & (
		TTF_STYLE_BOLD | TTF_STYLE_ITALIC | TTF_STYLE_UNDERLINE | TTF_STYLE_STRIKETHROUGH
	);


	TTF_SetFontStyle(fontptr, flags);

	return *this;
}

std::vector<std::string> ttf_representation::explode(
	const std::string & pstring, 
	const std::string& delimiter, 
	size_t max
) {

	size_t count=1, pos=0, prev_pos=0;

	std::vector<std::string> result;

	do {
		pos=pstring.find(delimiter, prev_pos);

		if(pos==std::string::npos) {
			result.push_back(pstring.substr(prev_pos));
		}
		else {
			result.push_back(pstring.substr(prev_pos, pos-prev_pos));
			prev_pos=pos+delimiter.size();
		}

		++count;
		if(max && count >= max) break;
	}
	while(pos!=std::string::npos);

	return result;
}
int ttf_representation::get_next_power_of_two(int _v) const {

	for(const auto& vs : valid_sizes) {
		if(_v > vs) continue;
		return vs;
	}

	throw std::runtime_error(std::string{"Invalid text texture size "}+std::to_string(_v)+std::string{" for text "}+text);
}

int ttf_representation::get_debug_flags() const {

#ifdef LIBDANSDL2_DEBUG
	return debug_flags;
#else
	return 0;
#endif
}

ttf_representation& ttf_representation::set_debug_flags(
#ifdef LIBDANSDL2_DEBUG
	int _flags
#else
	int
#endif
) {

#ifdef LIBDANSDL2_DEBUG
	debug_flags=_flags;
#endif
	return *this;
}
