#pragma once

#include "point_2d.h"
#include "tools.h"

#include <lm/log.h>
#include <SDL2/SDL.h>
#include <ostream>
#include <type_traits>

namespace ldt
{

//!A box.

enum class box_edges {top, bottom, left, right};

template<typename T, typename U> class box;

//These are less verbose ways to say "align".
template<typename T, typename U>
void snap_to_edge(box<T, U>&, T, box_edges);

template<typename T, typename U>
void snap_to_edge(box<T, U>&, const box<T, U>&, box_edges);

template<typename T, typename U>
void match_edge(box<T, U>&, const box<T, U>&, box_edges);

//Look at the actual definition below to make sense of this...
//template<typename T, typename U, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0 >
template<typename T, typename U>
void center_horizontally(box<T, U>&, T);

//template<typename T, typename U, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0 >
template<typename T, typename U>
void center_vertically(box<T, U>&, T);

template<typename T, typename U>
void center_horizontally(box<T, U>&, const box<T, U>&);

template<typename T, typename U>
void center_vertically(box<T, U>&, const box<T, U>&);

template<typename T, typename U>
point_2d<T> get_center(const box<T, U>&);

//!The only thing that is enforced here is that height goes towards the positive
//!infinite in the Y axis. This means "down" in screen coordinates and "up"
//!in cartesian ones. If this rule is respected, its functions should work.
template<typename T, typename U>
class box
{
	public:

	point_2d<T> 	origin;		//!< Box x and y components.
	U 		w, 		//!< Box width.
			h;		//!< Box height.

	//!Creates a box from a SDL rect. After all, this whole library is an
	//!SDL wrapper
	box(SDL_Rect r):
		origin{r.x,r.y}, w(r.w), h(r.h) {
	}

	//!Creates a box at 0,0, with no width or height.
	box():
		origin{0,0}, w(0), h(0) {
	}

	//!Creates a box from the point (x, y) and the given w and h components.
	box(const point_2d<T>& p, U pw, U ph)
		:origin(p), w(pw), h(ph) {
	}

	//!Creates a box from the given x, y, w and h components.
	box(T px, T py, U pw, U ph)
		:origin(px, py), w(pw), h(ph) {
	}

	//!Copy constructor.
	box(const box& c)
		:origin(c.origin), w(c.w), h(c.h) {
	}

	//!Assignment operator.
	box& operator=(const box& c) {
		origin=c.origin;
		w=c.w;
		h=c.h;
		return *this;
	}

	bool operator==(const box& _box) const {

		return _box.origin==origin
			&& _box.w==w
			&& _box.h==h;
	}

	bool operator!=(const box& _box) const {

		return _box.origin!=origin
			|| _box.w!=w
			|| _box.h!=h;
	}

/**
* Keeping the same center, grows the box (the origin will be displaced).
*/
	void            grow(T _amount) {

		T quantity=_amount / 2.;
		origin.x-=quantity;
		origin.y-=quantity;
		w+=_amount;
		h+=_amount;
	}

	/**
	* Adjusts the box so its edge _edge rests in _edge_position.
	*/
	void            snap_to_edge(box_edges _edge, T _edge_position) {

		ldt::snap_to_edge(*this, _edge_position, _edge);
	}

	/**
	* Adjusts the box so its edge _edge rests in the opposite _edge_position.
	*/
	void            snap_to_edge(const box& _obstacle, box_edges _edge) {

		ldt::snap_to_edge(*this, _obstacle, _edge);
	}

	/**
	* Adjusts the box so its edge _edge is the same as the one in _obstacle
	*/
	void            match_edge(const box& _obstacle, box_edges _edge) {

		ldt::match_edge(*this, _obstacle, _edge);
	}

	/**
	* Makes the horizontal center rest on _center.
	*/

	void            center_horizontally(T _center) {

		ldt::center_horizontally(*this, _center);
	}

	/**
	* Makes the horizontal center match the one on _obstacle.
	*/
	void            center_horizontally(const box& _obstacle) {

		ldt::center_horizontally(*this, _obstacle);
	}

	/**
	* Makes the vertical center rest on _center.
	*/
	void            center_vertically(T _center) {

		ldt::center_vertically(*this, _center);
	}

	/**
	* Makes the vertical center match the one on _obstacle.
	*/
	void            center_vertically(const box& _obstacle) {

		ldt::center_vertically(*this, _obstacle);
	}

	void            center(const box& _obstacle) {

		ldt::center_horizontally(*this, _obstacle);
		//ldt::center_vertically(*this, _obstacle);
	}

	//!Returns true if this box collides with the given box, edges touching 
	//!are not collisions, uses a epsilon value for precision errors.
	bool 			collides_with(const box& _c, T _epsilon) const {

		return ldt::rects_overlap(origin.x, origin.y, w, h, _c.origin.x, _c.origin.y, _c.w, _c.h, _epsilon);
	}

	//!Returns true if this box collides with the given box  The second
	//!parameter is legacy as it comes, governing whether a mere coincidence
	//!of edge values should be considered a collision.
	bool 			collides_with(const box& c, bool unit_is_collision=false) const {
		return ldt::rects_overlap(origin.x, origin.y, w, h, c.origin.x, c.origin.y, c.w, c.h, unit_is_collision);
	}

	//!Returns true if this box collides with the given box, returning the overlap in the second parameter
	//!passed by reference. The third parameter is legacy as it comes, governing whether a mere coincidence
	//!of edge values should be considered a collision.
	bool 			collides_with(const box& c, box& r, bool unit_is_collision=false) const {
		return ldt::rects_overlap(origin.x, origin.y, w, h, c.origin.x, c.origin.y, c.w, c.h, r.origin.x, r.origin.y, r.w, r.h, unit_is_collision);
	}

	//!Returns true if the given point is inside this box.
	bool 			point_inside(const point_2d<T>& p) const {
		return ldt::point_in_box(origin.x, origin.y, w, h, p.x, p.y);
	}

	//!Returns true if the given point is inside this box.
	bool 			point_inside(T x, T y) const {
		return ldt::point_in_box(origin.x, origin.y, w, h, x, y);
	}

	//!Returns true if this box is inside the given box (reverse of contains).
	bool 			inside(const box& gran) const {
		return ldt::box_in_box(origin.x, origin.y, w, h, gran.origin.x, gran.origin.y, gran.w, gran.h);
	}

	//!Returns true if this box contains the box indicated in the parameter (reverse of inside).
	bool 			contains(const box& peq) const {
		return ldt::box_in_box(peq.origin.x, peq.origin.y, peq.w, peq.h, origin.x, origin.y, w, h);
	}

	template<typename Y, typename Z>
	friend std::ostream& operator<<(std::ostream&, const box<Y, Z>&);

	template<typename Y, typename Z>
	friend lm::log& operator<<(lm::log&, const box<Y, Z>&);
};

template<typename T, typename U>
std::ostream& operator<<(
	std::ostream& _stream,
	const box<T, U>& _box
) {

	_stream<<_box.origin<<" "<<_box.w<<"x"<<_box.h;
	return _stream;
}

template<typename T, typename U>
lm::log& operator<<(
	lm::log& _log,
	const box<T, U>& _box
) {

	_log<<_box.origin<<" "<<_box.w<<"x"<<_box.h;
	return _log;
}

/**box
* Adjusts _box so its_edge rests in _edge_position. Edges make sense in
* cartesian space only.
*/
template<typename T, typename U>
void snap_to_edge(
	box<T, U>& _box,
	T _edge_position,
	box_edges _edge
) {

	switch(_edge) {

		case box_edges::top:
			_box.origin.y=_edge_position-_box.h;
			return;
		case box_edges::left:
			_box.origin.x=_edge_position;
			return;
		case box_edges::right:
			_box.origin.x=_edge_position-_box.w;
			return;
		case box_edges::bottom:
			_box.origin.y=_edge_position;
			return;
	}
}

/**
* Adjusts the box so its edge _edge rests in the opposite edge of _obstacle.
* Might make sense only in cartesian space.
*/
template<typename T, typename U>
void snap_to_edge(
	box<T, U>& _box,
	const box<T, U>& _obstacle,
	box_edges _edge
) {

	//TODO: can be implemented in terms of the other overload.

	switch(_edge) {

		case box_edges::top:
			_box.origin.y=_obstacle.origin.y-_box.h;
			return;
		case box_edges::left:
			_box.origin.x=_obstacle.origin.x+_obstacle.w;
			return;
		case box_edges::right:
			_box.origin.x=_obstacle.origin.x-_box.w;
			return;
		case box_edges::bottom:
			_box.origin.y=_obstacle.origin.y+_obstacle.h;
			return;
	}
}

/*
* Adjusts _box so its _edge matches the one in _obstacle. Might work only in
* cartesian space.
*/

template<typename T, typename U>
void match_edge(
	box<T, U>& _box,
	const box<T, U>& _obstacle,
	box_edges _edge
) {

	switch(_edge) {

		case box_edges::top:
			_box.origin.y=_obstacle.origin.y+_obstacle.h-_box.h;
			return;
		case box_edges::left:
			_box.origin.x=_obstacle.origin.x;
			return;
		case box_edges::right:
			_box.origin.x=_obstacle.origin.x+_obstacle.w-_box.w;
			return;
		case box_edges::bottom:
			_box.origin.y=_obstacle.origin.y;
			return;
	}
}

/**
 * what that last parameter means... if the type of T is arithmetic, 
 * add an argument here that is an int. The declaration above puts its 
 * default value at zero. This becomes a method with 3 args that gets only
 * instantiated when the type T is arithmetic, disambiguating a lot.
 */
template<
	typename T, 
	typename U
//	std::enable_if_t<std::is_arithmetic<T>::value, int>
>
void center_horizontally(
	box<T, U>& _box,
	T _center
) {

	_box.origin.x=_center-(_box.w/2);
}

template<typename T, typename U>
void center_horizontally(
	box<T, U>& _box,
	const box<T, U>& _container
) {

	T half=_container.w / 2;
	center_horizontally(_box, _container.origin.x+half);
}

template<
	typename T, 
	typename U
//	std::enable_if_t<std::is_arithmetic<T>::value, int>
>
void center_vertically(
	box<T, U>& _box,
	T _center
) {

	_box.origin.y=_center-(_box.h/2);
}

template<typename T, typename U>
void center_vertically(
	box<T, U>& _box,
	const box<T, U>& _container
) {

	T half=_container.h / 2;
	center_vertically(_box, _container.origin.y+half);
}

template<typename T, typename U>
void center(
	box<T, U>& _box,
	const box<T, U>& _container
) {

	center_horizontally(_box, _container);
	center_vertically(_box, _container);
}

/**
* Defines an alignment structure for boxes.
*/
struct alignment {

	//!Defines the types or horizontal aligment in human-readable format.
	enum class h{
		none,
		outer_left,
		inner_left,
		center,
		inner_right,
		outer_right} 	horizontal;	//!< Horizontal margin type.

	//!Defines the types or vertical aligment in human-readable format.
	enum class v{
		none,
		outer_top,
		inner_top,
		center,
		inner_bottom,
		outer_bottom} 	vertical; //!< Vertical margin type.

	alignment(h _h, v _v, int _hm=0, int _vm=0):
		horizontal(_h),
		vertical(_v),
		margin_horizontal(_hm),
		margin_vertical(_vm) {}

	int         margin_horizontal{0},	//!< Horizontal margin in px.
	            margin_vertical{0};	//!< Vertical margin in px.
};

/**
* Adjusts the box so it is aligned with the given "container" according to the
* specifications given in the alignment object.The margin indicates a separation
* from the given edge and can be negative.
*/
template<typename T, typename U>
void align(
	box<T, U>& _box,
	const box<T, U>& _container,
	alignment _alignment
) {
	auto pos=_box.origin;

	int mh=_alignment.margin_horizontal,
		mv=_alignment.margin_vertical;

	switch(_alignment.horizontal) {

		case alignment::h::none: break;
		case alignment::h::outer_left:
			pos.x=_container.origin.x-_box.w;
			mh=-mh;
		break;
		case alignment::h::inner_left:
			pos.x=_container.origin.x;
		break;
		case alignment::h::center:
			pos.x=(_container.origin.x+(_container.w / 2))-(_box.w/2);
		break;
		case alignment::h::inner_right:
			pos.x=_container.origin.x+_container.w-_box.w;
			mh=-mh;
		break;
		case alignment::h::outer_right:
			pos.x=_container.origin.x+_container.w;
		break;
	}

	switch(_alignment.vertical) {

		case alignment::v::none: break;
		case alignment::v::outer_top:
			pos.y=_container.origin.y+_container.h;
		break;
		case alignment::v::inner_top:
			pos.y=_container.origin.y+_container.h-_box.h;
			mv=-mv;
		break;
		case alignment::v::center:
			pos.y=(_container.origin.y+(_container.h / 2))-(_box.h/2);
		break;
		case alignment::v::inner_bottom:
			pos.y=_container.origin.y;
		break;
		case alignment::v::outer_bottom:
			pos.y=_container.origin.y-_box.h;
			mv=-mv;
		break;
	}

	pos.x+=mh;
	pos.y+=mv;
	_box.origin=pos;
}

template<typename T, typename U>
point_2d<T> get_center(const box<T, U>& _box) {

	point_2d<T> result{_box.origin};
	result.x+=_box.w / 2;
	result.y+=_box.h / 2;
	return result;
}

}

