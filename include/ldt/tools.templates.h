using namespace ldt;

#include <vector>
#include <algorithm>
#include <iostream>

//!Determines if two rectangles overlap. 

template<typename T, typename U> 
bool ldt::rects_overlap(
	T x1, 
	T y1, 
	U w1, 
	U h1, 
	T x2, 
	T y2, 
	U w2, 
	U h2, 
	bool unit_is_collision
) {

	T 	endx2=x2+w2,
		endx1=x1+w1,
		endy2=y2+h2,
		endy1=y1+h1;

	//There is an inherent problem here we cannot fix. If we are using doubles,
	//we can have a box at 244.0000000000003 with a width of 12 and have it
	//rounded at exactly 256. There is nothing we can do about this using 
	//floating point primitives, except believing that a collision did not
	//take place against an edge at 256.0 (if unit_is_collision=false).

	//Check if not colliding and negate...

	auto with_unit_collision=[](T p1, T p2, T e1, T e2) -> bool {return !(e2 < p1 || p2 > e1);};
	auto no_unit_collision=[](T p1, T p2, T e1, T e2) -> bool {return !(e2 <= p1 || p2 >= e1);};
	
	bool in_x=unit_is_collision ? with_unit_collision(x1, x2, endx1, endx2) : no_unit_collision(x1, x2, endx1, endx2),
		in_y=unit_is_collision ? with_unit_collision(y1, y2, endy1, endy2) : no_unit_collision(y1, y2, endy1, endy2);

	return in_x && in_y;
}

//!Determines if two rectangles overlap. Touching edges are not collisions. Uses epsilons to help with rounding errors.
/*
*What epsilon means here is the tolerance to overlap we have. If the overlap 
*is less than epsilon we consider no overlap to take place.
*e.g. consider endx1=101, x2=100:
* epsilon=0 this call would report an overlap:
*   not (101 <= 100 + 0) =>  not (false) => true => overlap.
* epsilon=0.5 we would have overlap.
*   not (101 <= 100 + 0.5) =>  not (false) => true => overlap.
* epsilon=1 we would have no overlap.
*   not (101 <= 100 + 1) =>  not (true) => false => no overlap.
*A good epsilon value can be 1e-9, which is very, very small.
*/

template<typename T, typename U> 
bool ldt::rects_overlap(
	T _x1, 
	T _y1, 
	U _w1, 
	U _h1, 
	T _x2, 
	T _y2, 
	U _w2, 
	U _h2, 
	T _epsilon
) {

	T 	endx2=_x2+_w2,
		endx1=_x1+_w1,
		endy2=_y2+_h2,
		endy1=_y1+_h1;

	return !(                       // there is not overlap if
			endx1 <= _x2 + _epsilon ||  // 1's right edge is to the left of 2's left...
			endx2 <= _x1 + _epsilon ||  // 2's right edge is to the lett of 1's left...
			endy1 <= _y2 + _epsilon ||  // 1's top is below 2's bottom...
			endy2 <= _y1 + _epsilon     // 2's top is below 1's bottom...
	);
}

//!Determines if two rectangles overlap and writes the overlap as a result.

//!When one box contains the other, the smaller one is considered the overlap.

template <typename T, typename U> 
bool ldt::rects_overlap(
	T x1, T y1, U w1, U h1, 
	T x2, T y2, U w2, U h2, 
	T& rx, T& ry, U& rw, U& rh, 
	bool unit_is_collision
) {
	if(!rects_overlap(x1, y1, w1, h1, x2, y2, h2, h2, unit_is_collision)) {
		return false;
	}
	else {
		T endx1=x1+w1;
		T endx2=x2+w2;
		T endy1=y1+h1;
		T endy2=y2+h2;

		std::vector<T> 	solution_x={x1, x2, endx1, endx2},
				solution_y={y1, y2, endy1, endy2};

		std::sort(std::begin(solution_x), std::end(solution_x));
		std::sort(std::begin(solution_y), std::end(solution_y));

		rx=solution_x[1];
		rw=solution_x[2]-rx;
		ry=solution_y[1];
		rh=solution_y[2]-ry;

		return true;
	}
}

//!Determines if a point is inside a box.

//!The function is coordinate system agnostic but in order to work with 
//!cartesian coordinates the height must extend upwards.

template <typename T, typename U> 
bool ldt::point_in_box(T cx, T cy, U cw, U ch, T px, T py) {
	auto in_limits=[](T p, T pos, T dim) -> bool {return p >= pos && p <= pos+dim;};
        return in_limits(px, cx, cw) && in_limits(py, cy, ch);
}

//!Determines if a box contains another box.

//!The function is coordinate system agnostic but in order to work with 
//!cartesian coordinates the height must extend upwards. The first set of
//!arguments builds a "small" box, while the second builds the "large" one.

template <typename T, typename U> 
bool ldt::box_in_box(T pqx, T pqy, U pqw, U pqh, T grx, T gry, U grw, U grh) {
	T big_xf=grx+grw,
	small_xf=pqx+pqw,
	big_yf=gry+grh,
	small_yf=pqy+pqh;

	return (grx <= pqx
	&& big_xf >= small_xf
	&& gry <= pqy
	&& big_yf >= small_yf);
}
