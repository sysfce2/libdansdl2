#include "representacion_agrupada.h"

using namespace DLibV;

Representacion_agrupada::Representacion_agrupada(bool p_poseer):
	posee_las_representaciones(p_poseer),
	impone_alpha(true),
	impone_modo_blend(false)
{
	this->reiniciar_posicion();
	this->reiniciar_recorte();
}

Representacion_agrupada::Representacion_agrupada(const Representacion_agrupada& p_otra):
	posee_las_representaciones(p_otra.posee_las_representaciones)
{
	if(!this->posee_las_representaciones)
	{
		this->grupo=p_otra.grupo;
	}

	this->reiniciar_posicion();
	this->reiniciar_recorte();
}

Representacion_agrupada::~Representacion_agrupada()
{
	vaciar_grupo();
}

void Representacion_agrupada::vaciar_grupo()
{
	if(this->posee_las_representaciones)
	{
		std::vector<Representacion *>::iterator ini=grupo.begin(),
						fin=grupo.end();
		while(ini < fin)
		{
			delete (*ini);
			ini++;
		};
	}

	grupo.clear();
}

/*
Representacion_agrupada& Representacion_agrupada::operator=(const Representacion_agrupada & p_otra)
{
	Representacion::operator=(p_otra);

	if(!posee_las_representaciones)
	{
		this->grupo=p_otra.grupo;
	}
	
	return *this;
}

*/
/*La posición que demos a la representación que se pasa se sumará luego a 
la posición del grupo. Para poner algo en la esquina superior izquierda del 
grupo estableceríamos la posición de rep en 0,0.
El alpha del grupo se aplica directamente a cada componente siempre que tenga
un valor distinto de "opaco".
Sería posible hacerlo "acumulativo" sin muchos problemas de modo que los valores
se sumen y resten dentro del rango 0-255.
*/

void Representacion_agrupada::volcado()
{
	std::vector<Representacion *>::iterator 	inicio=this->grupo.begin(),
							fin=this->grupo.end();

	Rect pos=acc_posicion();
	int px=pos.x, py=pos.y;
	unsigned int alpha_p=acc_alpha();
	auto modo_blend_p=acc_modo_blend();
//	unsigned int mod_color_r=acc_mod_color_r();
//	unsigned int mod_color_g=acc_mod_color_g();
//	unsigned int mod_color_b=acc_mod_color_b();

	unsigned int alpha_a=0;
	auto modo_blend_a=Representacion::blends::BLEND_NADA;

	while(inicio < fin)
	{
		if(impone_alpha)
		{
			alpha_a=(*inicio)->acc_alpha();
			(*inicio)->establecer_alpha(alpha_p);
		}

		if(impone_modo_blend)
		{
			modo_blend_a=(*inicio)->acc_modo_blend();
			(*inicio)->establecer_modo_blend(modo_blend_p);
		}

//		unsigned int mod_color_r_a=acc_mod_color_r();
//		unsigned int mod_color_g_a=acc_mod_color_g();
//		unsigned int mod_color_b_a=acc_mod_color_b();

		(*inicio)->desplazar(px, py);
//		(*inicio)->establecer_mod_color(mod_color_r, mod_color_g, mod_color_b);

//TODO... Where's the screen?. We cannot do this right now. The screen is a neccesary parameter.
//Maybe we could go with "volcado"... just for this, make it protected.
//Or maybe we could just think it over a little bit better.
//Or maybe we could get the screen as a parameter.
//		(*inicio)->volcar();

		(*inicio)->desplazar(-px, -py);
		if(impone_alpha) (*inicio)->establecer_alpha(alpha_a);
		if(impone_modo_blend) (*inicio)->establecer_modo_blend(modo_blend_a);
//		(*inicio)->establecer_mod_color(mod_color_r_a, mod_color_g_a, mod_color_b_a);

		++inicio;
	}
}

bool Representacion_agrupada::insertar_representacion(Representacion * p_rep)
{
	this->grupo.push_back(p_rep);
	return true;
}
