#ifndef _MUNDO__H_
#define _MUNDO__H_

#include "vetor.h"
#include "biota.h"

/******************************************************************************/

/* Noh de uma Fila de graos */
struct NohGrao
{
	Vetor<float> posicao;
	struct NohGrao *proximo;
};

/* Lista de paredes */
struct NohParede
{
	Vetor<float> inicio;
	Vetor<float> fim;
	struct NohParede *proximo;
};

class Mundo
{
protected:

	/* Lista de biotas */
	struct NohBiota
	{
		Biota biota;
		struct NohBiota *proximo;
	}
	nohCabecaBiotas;

	/* Noh do biota selecionado */
	struct NohBiota *selecionado;
	/* Noh do grao selecionado */
	struct NohGrao *grao_selecionado;

	struct NohParede nohCabecaParedes;

	/* Fila de graos */
	struct NohGrao nohCabecaGraos, *finalGraos;

	/* Operacoes com graos */
	/* remove um grao */
	bool removerGrao();

public:

	/* Estatisticas */
	struct
	{
		/* Contagem de biota e graos */
		unsigned int numero_biotas;
		unsigned int numero_graos;
		unsigned long int ciclos;
		unsigned long int mortes;
		unsigned long int nascimentos;
	}
	estatisticas;

	/* Propriedades globais */
	struct
	{
		unsigned int teto_energetico;
		unsigned int energia_grao;
		unsigned int probabilidade_mutacao;
		unsigned int intensidade_mutacao;
		unsigned int tamanho_x, tamanho_y;
	}
	propriedades;

	/* Construtores */
	Mundo();
	Mundo(unsigned int);
	Mundo(unsigned int, unsigned int);
	Mundo(unsigned int, unsigned int, unsigned int);

	/* Destructor */
	void destroy();

	/* Insere um biota no mundo */
	void inserirBiota(Biota);
	/* Atualiza todos os biotas e graos do mundo */
	void atualizar();

	/* Operacoes com biotas */
	bool selecionarBiota(Vetor<float>);
	void descelecionarBiota();

	/* Operacoes com graos */
	void inserirGrao(Vetor<float>);
	void posicionarGrao(Vetor<float>);
	bool selecionarGrao(Vetor<float>);
	void descelecionarGrao();
	void removerGraoSelecionado();
	bool graoSelecionado();

	/* abrir e salvar mundo */
	void salvarMundo(FILE *);
	void abrirMundo(FILE *);

	inline Biota getBiotaSelecionado()
	{
		if (selecionado)
			return selecionado->biota;
		else
			return Biota();
	}

	/* Classes amigas */
	friend class Biota;

	/* Funcoes amigas */
	void desenhar_mundo(Mundo*);
};

/******************************************************************************/

#endif /* #ifndef _MUNDO__H_ */
