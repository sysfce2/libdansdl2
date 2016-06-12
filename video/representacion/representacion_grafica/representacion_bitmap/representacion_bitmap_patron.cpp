#include "representacion_bitmap_patron.h"
using namespace DLibV;

Representacion_bitmap_patron::Representacion_bitmap_patron()
	:Representacion_bitmap(), 
	pincel{0,0,0,0}, pincel_x(0), pincel_y(0)
{

}

Representacion_bitmap_patron::Representacion_bitmap_patron(const Representacion_bitmap_patron& r)
	:Representacion_bitmap(r),
	pincel{0,0,0,0}, pincel_x(0), pincel_y(0)
{

}

Representacion_bitmap_patron::Representacion_bitmap_patron(const Textura * t)
	:Representacion_bitmap(t),
	pincel{0,0,0,0}, pincel_x(0), pincel_y(0)
{

}

Representacion_bitmap_patron& Representacion_bitmap_patron::operator=(const Representacion_bitmap_patron &o)
{
	Representacion_bitmap::operator=(o);
	pincel=o.pincel;
	pincel_x=0;
	pincel_y=0;

	return *this;
}

Representacion_bitmap_patron::~Representacion_bitmap_patron()
{

}

void Representacion_bitmap_patron::establecer_pincel(Rect r)
{
	pincel=r;
}

/*
* Llamaremos varias veces a la función de volcado original. Contaría como un
* único render para la cuenta de pantalla, pero me da igual.
*/

void Representacion_bitmap_patron::volcado()
{
	//TODO...
}

/*
* La llamada a preparar es un super hack... Al preparar cambiamos la posición
* para engañar a la función "volcado" base y decirle que la posición es el 
* trozo que estamos dibujando ahora. Después de volcarla la marcamos como no
* preparada para que haya que hacer el hack de nuevo.
*
* Todo eso está envuelto en otro hack más, que es hacer una copia de la posición
* que se restaura al final de la llamada. Esa copia se usará como caja de clip.
*/

/*
void Representacion_bitmap_patron::preparar(const SDL_Renderer * renderer)
{
	//Super hack..
	establecer_posicion(pincel_x, pincel_y, pincel.w, pincel.h);
	Representacion_grafica::preparar(renderer);
}
*/
