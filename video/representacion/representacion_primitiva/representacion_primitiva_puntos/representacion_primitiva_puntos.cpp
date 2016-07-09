#include "representacion_primitiva_puntos.h"
#include <algorithm>

using namespace DLibV;

Representacion_primitiva_puntos::Representacion_primitiva_puntos(int x, int y, ColorRGBA c)
	:Representacion_primitiva(c), original{0,0}
{
	insertar(x, y);
	actualizar_posicion_vista_rotacion();
}

Representacion_primitiva_puntos::Representacion_primitiva_puntos(ColorRGBA c)
	:Representacion_primitiva(c), original{0,0}
{
	actualizar_posicion_vista_rotacion();
}

Representacion_primitiva_puntos::Representacion_primitiva_puntos(const Representacion_primitiva_puntos& p_otra)
	:Representacion_primitiva(p_otra), puntos(p_otra.puntos), original(p_otra.original)
{

}

Representacion_primitiva_puntos& Representacion_primitiva_puntos::operator=(const Representacion_primitiva_puntos& p_otro)
{
	Representacion_primitiva::operator=(p_otro);
	puntos=p_otro.puntos;
	original=p_otro.original;
	return *this;
}

void Representacion_primitiva_puntos::insertar(int x, int y)
{
	if(!puntos.size())
	{
		original={x, y};
	}

	puntos.push_back({x-original.x, y-original.y});
	actualizar_posicion_vista_rotacion();
}

void Representacion_primitiva_puntos::limpiar_puntos()
{
	puntos.clear();
	original={0,0};
	actualizar_posicion_vista_rotacion();
}	

Rect Representacion_primitiva_puntos::obtener_base_posicion_vista() const
{
	if(!puntos.size())
	{
		return Rect{0,0,0,0};
	}
	else
	{
		//Del manual: 
		//The value returned indicates whether the element passed as first argument is considered less than the second.
		struct {bool operator() (Punto a, Punto b) {return a.x < b.x;}}fx;
		struct {bool operator() (Punto a, Punto b) {return a.y < b.y;}}fy;

		auto min_x=*std::min_element(std::begin(puntos), std::end(puntos), fx);
		auto max_x=*std::max_element(std::begin(puntos), std::end(puntos), fx);
		auto min_y=*std::min_element(std::begin(puntos), std::end(puntos), fy);
		auto max_y=*std::max_element(std::begin(puntos), std::end(puntos), fy);

		return Rect{min_x.x+original.x, min_y.y+original.y, (unsigned int)max_x.x-min_x.x+original.x, (unsigned int)max_y.y-min_y.y+original.y};
	}
}

void Representacion_primitiva_puntos::volcado()
{
	preparar_color();
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_INT, 0, puntos.data());
	glDrawArrays(GL_POINTS, 0, puntos.size());
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Representacion_primitiva_puntos::ir_a(int x, int y)
{
	original.x=x;
	original.y=y;
	actualizar_posicion_vista_rotacion();
}

Punto Representacion_primitiva_puntos::obtener_posicion() const
{
	struct {bool operator() (Punto a, Punto b) {return a.x < b.x;}}fx;
	struct {bool operator() (Punto a, Punto b) {return a.y < b.y;}}fy;

	auto min_x=*std::min_element(std::begin(puntos), std::end(puntos), fx);
	auto min_y=*std::min_element(std::begin(puntos), std::end(puntos), fy);

	return Punto{min_x.x+original.x, min_y.y+original.y};
}
