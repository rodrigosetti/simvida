#ifndef _DEFINICOES__H_
#define _DEFINICOES__H_

/******************************************************************************/

enum ResultadoAtualizacao
{
	REPRODUCAO, MORTE, NORMAL
};

/******************************************************************************/

/*
	Fatores de PI
*/
const float DPI = 6.283185307179586476925286766559f;
const float PI = 3.1415926535897932384626433832795f;
const float MPI = 1.5707963267948966192313216916398f;
const float QPI = 0.78539816339744830961566084581988f;

/******************************************************************************/

/* Precisao da posicao de selecao em relacao ao raio da cabeca */
const int FATOR_SELECAO = 2;

const float FATOR_PERCEPCAO_GRAOS = 16.0f;
/* Proporcao do giro dos segmentos transferida ao giro da cabeca */
const float PERDA_ENERGIA_CABECA = 0.06f;
/* Aceleracao angular aplicada para mover-se em direcao a comida */
const float ACEL_GIRO = 0.06f;
/* Raio do grao de comida */
const int RAIO_GRAO = 6;
/* Fator de velocidade do segmento */
const float VEL_SEGMENTO = 1.0f;
const float FLUIDEZ = 0.0006f;
const float FREIO_IMPULSO = 0.0078125f;
const float VISCOSIDADE = 6.0f;
const float DISSIPACAO_ENERGETICA = 0.0004f;
const float ATRITO = 0.9f;
const float PERDA_ENERGETICA_IDADE = 0.0001f;

const int ENERGIA_INICIAL_PADRAO = 200;

const float ATRITO_ANGULAR = 0.8f;

const int DISTANCIA_SELECAO_PAREDE = 20;

const int GROSSURA_PAREDE = 8;

/******************************************************************************/

#endif /* #ifndef _DEFINICOES__H_ */
