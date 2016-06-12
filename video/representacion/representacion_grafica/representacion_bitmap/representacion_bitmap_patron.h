#ifndef REPRESENTACION_BITMAP_PATRON_H
#define REPRESENTACION_BITMAP_PATRON_H

#include "representacion_bitmap.h"

/*
* Una representación de bitmap que se repite. "posicion" hará del área de 
* relleno, "recorte" será la porción de textura a recortar y "pincel" el tamaño 
* en que este recorte se aplicará hasta rellenar todo el área. X e Y del
* pincel se usan para "desplazar" el inicio.
*
* Su función de volcado es específica.
*/

namespace DLibV
{

class Representacion_bitmap_patron:
	public Representacion_bitmap
{
	public:

					Representacion_bitmap_patron();
					Representacion_bitmap_patron(const Representacion_bitmap_patron&);
					Representacion_bitmap_patron(const Textura *);
					Representacion_bitmap_patron& operator=(const Representacion_bitmap_patron &);
					virtual ~Representacion_bitmap_patron();

	void				establecer_pincel(Rect);
	Rect			acc_pincel() const {return pincel;}
	virtual Rect		obtener_caja_clip() const {return caja_clip;}

	protected:

	virtual void 			volcado();

	private:

	Rect			pincel;
	Rect			caja_clip;
	int				pincel_x, pincel_y;
};
}
#endif
