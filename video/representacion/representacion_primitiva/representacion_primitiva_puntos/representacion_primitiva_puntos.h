#ifndef PRIMITIVA_GRAFICA_PUNTOS_H
#define PRIMITIVA_GRAFICA_PUNTOS_H

#include "../representacion_primitiva.h"
#include <vector>

namespace DLibV
{
class Representacion_primitiva_puntos:
	public Representacion_primitiva
{
	public:
	
						Representacion_primitiva_puntos(ColorRGBA);
						Representacion_primitiva_puntos(int, int, ColorRGBA);
						Representacion_primitiva_puntos(const Representacion_primitiva_puntos&);
	virtual 				~Representacion_primitiva_puntos() {}
						Representacion_primitiva_puntos& operator=(const Representacion_primitiva_puntos&);

	virtual void				volcado(SDL_Renderer *);
	virtual void				volcado(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, double); 

	void 					insertar(int x, int y);
	void					limpiar_puntos();

	protected:

	void 					generar_posicion();
	void 					preparar_posicion();


	private:

	std::vector<Representacion_primitiva::punto>			puntos;
};

}

#endif
