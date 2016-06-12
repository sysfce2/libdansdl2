#ifndef CAMARA_H
#define CAMARA_H

#include <SDL2/SDL.h>
#include "../rect/rect.h"
#include "../../herramientas/herramientas_sdl/herramientas_sdl.h"

namespace DLibV
{

class Camara
{
	public:

	//Interfaz pública.

				Camara(int=0, int=0, unsigned int=800, unsigned int=600, unsigned int=0, unsigned int=0);

	const Rect& 		acc_caja_foco() const {return this->caja_foco;}
	const Rect& 		acc_caja_pos() const {return this->caja_pos;}

	Rect 			copia_caja_foco() const {return this->caja_foco;}
	Rect	 		copia_caja_pos() const {return this->caja_pos;}

	double			acc_zoom() const {return zoom;}
	unsigned int		acc_pos_x() const {return this->pos_x;}
	unsigned int 		acc_pos_y() const {return this->pos_y;}
	unsigned int		acc_pos_w() const {return this->caja_pos.w;}
	unsigned int 		acc_pos_h() const {return this->caja_pos.h;}
	unsigned int		acc_foco_w() const {return this->caja_foco.w;}
	unsigned int 		acc_foco_h() const {return this->caja_foco.h;}
	int			acc_x() const {return this->caja_foco.x;}
	int 			acc_y() const {return this->caja_foco.y;}

	void 			mut_pos_x(unsigned int p_valor) {this->pos_x=p_valor; sincronizar_cajas();}
	void 			mut_pos_y(unsigned int p_valor) {this->pos_y=p_valor; sincronizar_cajas();}
	void 			mut_x(int p_valor) {this->caja_foco.x=p_valor;}
	void 			mut_y(int p_valor) {this->caja_foco.y=p_valor;}
	void 			mut_w_pos(unsigned int v) {caja_pos.w=v;}
	void 			mut_h_pos(unsigned int v) {caja_pos.h=v;}
	void 			mut_zoom(double);

	void 			enfocar_a(int, int);
	void 			movimiento_relativo(int=0, int=0);
	void 			establecer_limites(int, int, int, int);
	void 			limpiar_limite();

	void 			transformar_posicion_raton(int& x, int& y);

	private:

	void 			sincronizar_cajas();

	Rect 			caja_foco;			//La caja a la que se apunta en el "mundo", es decir, estamos apuntado al punto x, y, enfocando w y h del mundo. Si cambian estos w y h haríamos zoom.
	Rect	 		caja_pos;		//La caja desplazada con pos_x y pos_y, guardada aquí para no crearla again and again. La cámara aparece en pantalla en x e y, con ancho y alto tal.
	unsigned int 		pos_x;		//La posición X e Y en la que aparece la cámara en pantalla.
	unsigned int 		pos_y;


	bool 			limitada;
	int 			limite_min_x;		//Límites para el movimiento de la cámara.
	int 			limite_min_y;
	int 			limite_max_x;
	int 			limite_max_y;
	double 			zoom;

};

} //Fin namespace DLibV

#endif
