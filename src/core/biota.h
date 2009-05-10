/** \file
    Definicao da classe Biota
    \author Rodrigo Setti
*/
#ifndef _BIOTA__H_
#define _BIOTA__H_

#include <stdio.h>
#include <QDomNode>

#include "definicoes.h"
#include "vetor.h"

/******************************************************************************/

/**
    Estrutura que define os genes de um segmento
    \author Rodrigo Setti
*/
struct GenesSegmento
{
	float arco;
	int comprimento;
	int massa;
	float angulo;
	unsigned int cor[3];
	float fa, fb;
};

/******************************************************************************/

/**
    Estrutura que define os genes de um biota
    \author Rodrigo Setti
*/
struct GenesBiota
{
	struct GenesSegmento *segmentos;
	unsigned int cor_cabeca[3];
	int massa_cabeca;
	int limiar_reproducao;
	float distribuicao_energia;
	float angulo_reproducao;
	float angulo_colisao;
};

/******************************************************************************/

/**
    Estrutura que define o estado de um biota
    \author Rodrigo Setti
*/
struct EstadoBiota
{
	float *posicaoSegmentos;
	Vetor<float> posicao;
	Vetor<float> velocidade;
	float velocidade_angular;
	float angulo;
	float energia;
	unsigned int idade;
	unsigned int geracao;
	unsigned int filhos;
};

/******************************************************************************/

/**
    Classe que define o proprio biota
    \author Rodrigo Setti
*/
class Biota
{

public:
	unsigned int lineage;

	/** Estrutura que define os genes de um biota */
	struct GenesBiota genes;

	/** Estrutura que define o estado de um biota */
	struct EstadoBiota estado;

	unsigned int numero_segmentos;

	/** Referencia ao mundo que o biota vive */
	void *mundo;

//public:

	/* Construtores */
	Biota();
	Biota(void*);
	Biota(const Biota&);
	Biota(void*, Vetor<float>, FILE*);

	/* Metodos */
	enum ResultadoAtualizacao atualizar(void*);
	void mutacao();
	bool selecionar(Vetor<float>);
	void salvar(FILE*, bool saveState=false, int ident=0);
	void abrir(FILE*);
	void abrir(QDomNode xmlNode);
	void posicionar(Vetor<float>);
	void acelerar(Vetor<float>);
	void destroy();
	void refletir();

	/* Amigos */
	friend class Mundo;
};

/******************************************************************************/

#endif /* #ifndef _BIOTA__H_ */
