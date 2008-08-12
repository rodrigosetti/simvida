#ifndef _VETOR__H
#define _VETOR__H

#include <math.h>

/*
	Classe Vetor : Interface

	Encapsula a estrutura de dados e operacoes aplicaveis em um vetor bidimensional.

	rodrigosetti@gmail.com
*/
template <class TIPO>
class Vetor
{
public:
	TIPO X, Y;

	/* Construtores */
	Vetor();
	Vetor(const float, const float);


	/* Operacoes comuns*/

	Vetor operator +(const Vetor);
	Vetor operator -(const Vetor);
	Vetor operator *(const Vetor);
	Vetor operator /(const Vetor);

	Vetor operator +=(const Vetor);
	Vetor operator -=(const Vetor);
	Vetor operator *=(const Vetor);
	Vetor operator /=(const Vetor);

	Vetor operator +(const TIPO);
	Vetor operator -(const TIPO);
	Vetor operator *(const TIPO);
	Vetor operator /(const TIPO);

	Vetor operator +=(const TIPO);
	Vetor operator -=(const TIPO);
	Vetor operator *=(const TIPO);
	Vetor operator /=(const TIPO);
	Vetor operator =(const TIPO);

	Vetor operator -();

	/* Operacoes de comparacao */

	bool operator ==(const Vetor);
	bool operator !=(const Vetor);

	/* Opracoes avancadas */

	Vetor reduzir();
	float modulo();
	float distancia(Vetor);
	bool esquerda(Vetor, Vetor);
};

/*
	Classe Vetor : Implementacao

	Encapsula a estrutura de dados e operacoes aplicaveis em um vetor bidimensional.

	rodrigosetti@gmail.com
*/

/* Construtores */

template <class TIPO>
Vetor<TIPO>::Vetor()
{
	X = 0;
	Y = 0;
}

template <class TIPO>
Vetor<TIPO>::Vetor(float x, float y)
{
	X = x;
	Y = y;
}

/* Operacoes comuns*/

#define OPERATOR_FUNC(op)								\
	template <class TIPO>                   			\
	Vetor<TIPO> inline Vetor<TIPO>::operator op(Vetor A)\
	{													\
		return Vetor<TIPO>(X op A.X, Y op A.Y);			\
	}

OPERATOR_FUNC(+)
OPERATOR_FUNC(-)
OPERATOR_FUNC(*)
OPERATOR_FUNC(/)
OPERATOR_FUNC(+=)
OPERATOR_FUNC(-=)
OPERATOR_FUNC(*=)
OPERATOR_FUNC(/=)

#undef OPERATOR_FUNC
#define OPERATOR_FUNC(op)								\
	template <class TIPO>   		            		\
	Vetor<TIPO> inline Vetor<TIPO>::operator op(TIPO c)	\
	{													\
		return Vetor<TIPO>(X op c, Y op c);				\
	}

OPERATOR_FUNC(+)
OPERATOR_FUNC(-)
OPERATOR_FUNC(*)
OPERATOR_FUNC(/)
OPERATOR_FUNC(+=)
OPERATOR_FUNC(-=)
OPERATOR_FUNC(*=)
OPERATOR_FUNC(/=)
OPERATOR_FUNC(=)

template <class TIPO>
Vetor<TIPO> Vetor<TIPO>::operator -()
{
	return Vetor<TIPO>(-X,-Y);
}

/* Operacoes de comparacao */

template <class TIPO>
bool inline Vetor<TIPO>::operator ==(Vetor A)
{
	return (X == A.X && Y == A.Y);
}

template <class TIPO>
bool inline Vetor<TIPO>::operator !=(Vetor A)
{
	return (X != A.X || Y != A.Y);
}

/* Opracoes avancadas */

template <class TIPO>
Vetor<TIPO> inline Vetor<TIPO>::reduzir()
{
	return Vetor<TIPO>(X / sqrt((X*X)+(Y*Y)), Y / sqrt((X*X)+(Y*Y)));
}

template <class TIPO>
float inline Vetor<TIPO>::modulo()
{
	return sqrt((X*X)+(Y*Y));
}

template <class TIPO>
float inline Vetor<TIPO>::distancia(Vetor A)
{
	return sqrt(((X - A.X)*(X - A.X)) + ((Y - A.Y)*(Y - A.Y)));
}


#endif /* #ifndef _VETOR__H */
