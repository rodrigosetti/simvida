#include "mundo.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>

/******************************************************************************/

/* Operacoes com graos */
/* Acrescenta um novo grao na posicao dada */
void Mundo::inserirGrao(Vetor<float> posicao)
{
	struct NohGrao *novoNohGrao = new NohGrao;

	/* Percorre lista de paredes */
	for (struct NohParede *percorre = nohCabecaParedes.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		Vetor<float> u = posicao - percorre->inicio;
		Vetor<float> v = percorre->fim - percorre->inicio;
		Vetor<float> proj = (v.modulo () == 0)? posicao : (
		(v / v.modulo ()) *
		(((u.X * v.X)+(u.Y * v.Y)) / v.modulo()) );

		proj += percorre->inicio;

		/* Corrige limites da parede */
		if (proj.X > percorre->inicio.X && proj.X > percorre->fim.X)
			proj = (percorre->inicio.X > percorre->fim.X)? percorre->inicio : percorre->fim;
		else if (proj.X < percorre->inicio.X && proj.X < percorre->fim.X)
			proj = (percorre->inicio.X < percorre->fim.X)? percorre->inicio : percorre->fim;
		else if (proj.Y > percorre->inicio.Y && proj.Y > percorre->fim.Y)
			proj = (percorre->inicio.Y > percorre->fim.Y)? percorre->inicio : percorre->fim;
		else if (proj.Y < percorre->inicio.Y && proj.Y < percorre->fim.Y)
			proj = (percorre->inicio.Y < percorre->fim.Y)? percorre->inicio : percorre->fim;

		/* Testa distancia */
		if (posicao.distancia(proj) < GROSSURA_PAREDE + RAIO_GRAO)
		{
			/* Altera posicao do grao */
			/* Calcula angulo de "colisao" */
			float ang = (posicao.Y - proj.Y == 0 && posicao.X - proj.X == 0)? 0.0f :
			atan2(posicao.Y - proj.Y, posicao.X - proj.X);

			/* Reajusta posicao */
			posicao += Vetor<float>(cos(ang),sin(ang)) *
			((GROSSURA_PAREDE + RAIO_GRAO) - posicao.distancia(proj));
		}
	}

	novoNohGrao->posicao = posicao;
	novoNohGrao->proximo = NULL;

	finalGraos->proximo = novoNohGrao;
	finalGraos = novoNohGrao;

	estatisticas.numero_graos++;
}

/******************************************************************************/

/* Remove um grao da lista */
bool Mundo::removerGrao()
{
	if (nohCabecaGraos.proximo == NULL)
		return false;

	struct NohGrao *remover = nohCabecaGraos.proximo;
	nohCabecaGraos.proximo = nohCabecaGraos.proximo->proximo;

	/* Corrige caso se tente remover o fim da lista */
	if (remover == finalGraos)
		finalGraos = &nohCabecaGraos;

	/* Corrige caso seja selecionado */
	if (remover == grao_selecionado)
		grao_selecionado = NULL;

	delete remover;

	estatisticas.numero_graos--;

	return true;
}

/******************************************************************************/

/* Construtores */
Mundo::Mundo()
{
	srand(time(NULL));

	propriedades.teto_energetico = 30000;
	propriedades.energia_grao = 200;
	propriedades.probabilidade_mutacao = 10;
	propriedades.intensidade_mutacao = 1;
	propriedades.tamanho_x = 800;
	propriedades.tamanho_y = 600;

	estatisticas.numero_biotas = 0;
	estatisticas.numero_graos = 0;
	estatisticas.ciclos = 0;
	estatisticas.nascimentos = 0;
	estatisticas.mortes = 0;

	selecionado = NULL;
	grao_selecionado = NULL;
	nohCabecaBiotas.proximo = NULL;
	nohCabecaGraos.proximo = NULL;
	nohCabecaParedes.proximo = NULL;
	finalGraos = &nohCabecaGraos;
}

/******************************************************************************/

/* Constroi mundo com tamanho definido */
Mundo::Mundo(unsigned int tx, unsigned int ty)
{
	Mundo();
	propriedades.tamanho_x = tx;
	propriedades.tamanho_y = ty;
}

/******************************************************************************/

/* Constroi mundo com determinado numero de biotas aleatorios */
Mundo::Mundo(unsigned int numero_de_biotas)
{
	Mundo();
	for (unsigned int c = 0; c < numero_de_biotas; c++)
	{
		inserirBiota(Biota(this));
	}
}

/******************************************************************************/

/* Constroi mundo com tamanho definido e determinado numero de biotas */
Mundo::Mundo(unsigned int tx, unsigned int ty, unsigned int numero_de_biotas)
{
	Mundo();
	propriedades.tamanho_x = tx;
	propriedades.tamanho_y = ty;
	for (unsigned int c = 0; c < numero_de_biotas; c++)
	{
		inserirBiota(Biota(this));
	}
}

/******************************************************************************/

/* Destrutor : limpa biotas e graos*/
void Mundo::destroy()
{
	/* Remove todos biotas */
	while (nohCabecaBiotas.proximo != NULL)
	{
		struct NohBiota *remover = nohCabecaBiotas.proximo;
		nohCabecaBiotas.proximo = nohCabecaBiotas.proximo->proximo;

		/* Libera memoria */
		remover->biota.destroy();
		delete remover;
	}

	/* Remove todas as paredes */
	while (nohCabecaParedes.proximo != NULL)
	{
		struct NohParede *remover = nohCabecaParedes.proximo;
		nohCabecaParedes.proximo = nohCabecaParedes.proximo->proximo;

		/* Libera memoria */
		delete remover;
	}

	/* Remove todos graos */
	while (removerGrao());

	/* Zera estatisticas */
	estatisticas.numero_biotas = 0;
	estatisticas.numero_graos = 0;
	estatisticas.ciclos = 0;
	estatisticas.mortes = 0;
	estatisticas.nascimentos = 0;
}

/******************************************************************************/

/* Operacoes */
void Mundo::inserirBiota(Biota biota)
{
	struct NohBiota *novoNohBiota = new NohBiota;
	biota.mundo = this;

	novoNohBiota->biota = biota;
	novoNohBiota->proximo = nohCabecaBiotas.proximo;

	nohCabecaBiotas.proximo = novoNohBiota;
	estatisticas.numero_biotas++;
}

/******************************************************************************/

/* Atualiza mundo */
void Mundo::atualizar()
{
	/* Contador de energia total dos biotas */
	float energia_biotas = 0;

	/* Percorre a lista de biotas */
	struct NohBiota *percorre = &nohCabecaBiotas;
	while(percorre->proximo != NULL)
	{
		Biota *novoBiota;

		/* Acrescenta energia ao contador */
		energia_biotas += percorre->proximo->biota.estado.energia;

		/* Atualiza biota e trata resultado */
		switch (percorre->proximo->biota.atualizar((void*)percorre->proximo))
		{
			case REPRODUCAO:
				/* Cria biota */
				novoBiota = new Biota(percorre->proximo->biota);
				/* Distribui energia */
				novoBiota->estado.energia =
				percorre->proximo->biota.estado.energia *
				percorre->proximo->biota.genes.distribuicao_energia;
				percorre->proximo->biota.estado.energia -=
				novoBiota->estado.energia;
				/* Modifica angulo de reproducao */
				novoBiota->estado.angulo +=
				percorre->proximo->biota.genes.angulo_reproducao;
				/* Seta geracao  e idade*/
				novoBiota->estado.geracao++;
				novoBiota->estado.idade = 0;
				/* Seta numero de filhos de cada um */
				novoBiota->estado.filhos = 0;
				if (percorre->proximo->biota.estado.filhos++ % 2 == 0)
					novoBiota->refletir();
				/* Aplica provavel mutacao */
				if ((unsigned int)(rand() % 100) <= propriedades.probabilidade_mutacao)
					novoBiota->mutacao();
				/* Insere biota na lista */
				inserirBiota(*novoBiota);
				/* Corrige atualizacao imediata do recem-criado */
				if (percorre == &nohCabecaBiotas)
					percorre = percorre->proximo;
				/* Atualiza estatistica */
				estatisticas.nascimentos++;
				/* NAO POSSUI BREAK : entra no normal */
			case NORMAL:
				/*
				 	Ambas reproducao e normal devem andar
					na lista, morte nao, porque retira.
				 */
				percorre = percorre->proximo;
				 break;
			case MORTE:
				/* Remove biota da lista */
				struct NohBiota *remover = percorre->proximo;
				percorre->proximo = percorre->proximo->proximo;
				/* Verifica se biota esta selecionado */
				if (selecionado == remover)
					selecionado = NULL;
				/* Determina estatisticas de morte */
				estatisticas.mortes++;
				/* Libera memoria */
				remover->biota.destroy();
				delete remover;
				/* Decrementa contador de biotas */
				estatisticas.numero_biotas--;
				break;
		}
	}

	/* Incrementa ciclos */
	estatisticas.ciclos++;

	/* Move grao aleatoriamente, se maior que limiar */
	if (estatisticas.numero_graos > (propriedades.teto_energetico + propriedades.energia_grao)/2)
	{
		removerGrao();
		inserirGrao(Vetor<float>((GROSSURA_PAREDE + RAIO_GRAO) + (rand() % (propriedades.tamanho_x-(2*(GROSSURA_PAREDE + RAIO_GRAO)))),
		 (GROSSURA_PAREDE + RAIO_GRAO) + (rand() % (propriedades.tamanho_y-(2*(GROSSURA_PAREDE + RAIO_GRAO))))));
	}

	/* Trata incremento ou decremento de graos do sistema */
	if (energia_biotas + (estatisticas.numero_graos * propriedades.energia_grao) >
	propriedades.teto_energetico + propriedades.energia_grao)
		removerGrao();
	else if (energia_biotas + (estatisticas.numero_graos * propriedades.energia_grao) <
	propriedades.teto_energetico - propriedades.energia_grao)
		inserirGrao(Vetor<float>((GROSSURA_PAREDE + RAIO_GRAO) + (rand() % (propriedades.tamanho_x-(2*(GROSSURA_PAREDE + RAIO_GRAO)))),
		 (GROSSURA_PAREDE + RAIO_GRAO) + (rand() % (propriedades.tamanho_y-(2*(GROSSURA_PAREDE + RAIO_GRAO))))));
}

/******************************************************************************/

/* Operacoes com biotas */
bool Mundo::selecionarBiota(Vetor<float> posicao)
{
	/* Percorre a lista de biotas */
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (percorre->biota.selecionar(posicao))
		{
			selecionado = percorre;
			return true;
		}
	}

	return false;
}

/******************************************************************************/

void Mundo::descelecionarBiota()
{
	selecionado = NULL;
}

/******************************************************************************/
/* OPERACOES COM GRAOS */
/******************************************************************************/

void Mundo::removerGraoSelecionado()
{
	if (grao_selecionado == NULL)
		return;

	/* Percorre a lista de graos */
	for (struct NohGrao *percorre = &nohCabecaGraos;
	percorre->proximo != NULL;
	percorre = percorre->proximo)
	{
		if (percorre->proximo == grao_selecionado)
		{
			struct NohGrao *remove = percorre->proximo;
			percorre->proximo = percorre->proximo->proximo;
			delete remove;
			grao_selecionado = NULL;

			return;
		}
	}
}

/******************************************************************************/

void Mundo::descelecionarGrao()
{
	grao_selecionado = NULL;
}

/******************************************************************************/

bool Mundo::selecionarGrao(Vetor<float> posicao)
{
	/* Percorre a lista de graos */
	for (struct NohGrao *percorre = nohCabecaGraos.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (percorre->posicao.distancia(posicao) < RAIO_GRAO)
		{
			grao_selecionado = percorre;
			return true;
		}
	}

	return false;
}

/******************************************************************************/

void Mundo::posicionarGrao(Vetor<float> posicao)
{
	if (grao_selecionado != NULL)
	{
		grao_selecionado->posicao = posicao;
	}
}

/******************************************************************************/

bool Mundo::graoSelecionado()
{
	return (grao_selecionado != NULL);
}

/******************************************************************************/
void Mundo::salvarParedes(FILE *arq)
{
	/* Percorre lista de paredes */
	for (struct NohParede *percorre = nohCabecaParedes.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		/* Escreve no arquivo */
		fprintf(arq, "%d,%d:%d,%d\n",
		(int)(percorre->inicio.X), (int)(percorre->inicio.Y),
		(int)(percorre->fim.X), (int)(percorre->fim.Y));
	}
}

/******************************************************************************/

void Mundo::abrirParedes(FILE *arq)
{
	while (nohCabecaParedes.proximo != NULL)
	{
		struct NohParede *remover = nohCabecaParedes.proximo;
		nohCabecaParedes.proximo = nohCabecaParedes.proximo->proximo;

		/* Libera memoria */
		delete remover;
	}

	/* Le do arquivo e insere paredes */
	while (!feof(arq))
	{
		int ax, ay, bx, by;
		fscanf(arq, "%d,%d:%d,%d\n", &ax, &ay, &bx, &by);

		/* Insere nova parede */
		struct NohParede *novaParede = new struct NohParede;

		novaParede->inicio = Vetor<float>(ax,ay);
		novaParede->fim = Vetor<float>(bx,by);
		novaParede->proximo = nohCabecaParedes.proximo;
		nohCabecaParedes.proximo = novaParede;
	}
}

/******************************************************************************/

void Mundo::salvarMundo(FILE *arq)
{
	/* Salva estatisticas */
	fprintf(arq, "%lu,%lu,%lu\n",	 
	estatisticas.ciclos, estatisticas.mortes, estatisticas.nascimentos);

	/* salva propriedades */
	fprintf(arq, "%d,%d,%d,%d,%d,%d\n",
	propriedades.teto_energetico, propriedades.energia_grao, 
	propriedades.probabilidade_mutacao, propriedades.intensidade_mutacao,
	propriedades.tamanho_x, propriedades.tamanho_y);

	/* Salva graos */
	for (struct NohGrao *percorre = nohCabecaGraos.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		fprintf(arq, "(%d,%d)\n", (int)percorre->posicao.X, (int)percorre->posicao.Y);
	}
	
	/* separador */
	fprintf(arq, "#\n");

	/* Salvar biotas */
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		/* salva correcao anti-leudemais */
		fprintf(arq, "[");
		/* salva genes */
		percorre->biota.salvar(arq);
		/* salva estado */
		fprintf(arq, "[(%f,%f)(%f,%f),%f,%f,%f,%d,%d,%d,%d]\n",
		percorre->biota.estado.posicao.X, percorre->biota.estado.posicao.Y,
		percorre->biota.estado.velocidade.X, percorre->biota.estado.velocidade.Y,
		percorre->biota.estado.velocidade_angular, percorre->biota.estado.angulo,
		percorre->biota.estado.energia, percorre->biota.estado.idade,
		percorre->biota.estado.geracao, percorre->biota.estado.filhos,
		percorre->biota.lineage);
		fprintf(arq, "(");
		for (unsigned int c = 0; c < percorre->biota.numero_segmentos; c++)
			fprintf(arq, "%f,", percorre->biota.estado.posicaoSegmentos[c]);
		fprintf(arq, ")\n");			
	}

	/* separador */
	fprintf(arq, "#\n");

	/* Salvar paredes */
	salvarParedes(arq);
}

/******************************************************************************/

void Mundo::abrirMundo(FILE *arq)
{
	/* limpa mundo */
	destroy();

	/* Abre estatisticas */
	fscanf(arq, "%lu,%lu,%lu\n",
	&estatisticas.ciclos, &estatisticas.mortes, &estatisticas.nascimentos);

	/* Abre propriedades */
	fscanf(arq, "%d,%d,%d,%d,%d,%d\n",
	&propriedades.teto_energetico, &propriedades.energia_grao, 
	&propriedades.probabilidade_mutacao, &propriedades.intensidade_mutacao,
	&propriedades.tamanho_x, &propriedades.tamanho_y);

	/* Abre graos */
	while (fgetc(arq) != '#')
	{
		int px, py;
		fscanf(arq, "%d,%d)\n", &px, &py);
		inserirGrao(Vetor<float>(px,py));		
	}
	fgetc(arq);

	/* Abre biotas */
	while (fgetc(arq) != '#')
	{
		/* le genes */
		Biota biota = Biota(this, Vetor<float>(), arq);
		/* le estado */
		fscanf(arq, "[(%f,%f)(%f,%f),%f,%f,%f,%d,%d,%d,%d]\n",
		&biota.estado.posicao.X, &biota.estado.posicao.Y,
		&biota.estado.velocidade.X, &biota.estado.velocidade.Y,
		&biota.estado.velocidade_angular, &biota.estado.angulo,
		&biota.estado.energia, &biota.estado.idade,
		&biota.estado.geracao, &biota.estado.filhos, &biota.lineage);

		fscanf(arq, "(");
		for (unsigned int c = 0; c < biota.numero_segmentos; c++)
			fscanf(arq, "%f,", &biota.estado.posicaoSegmentos[c]);
		fscanf(arq, ")\n");	

		/* adiciona */
		inserirBiota(biota);
	}
	fgetc(arq);

	/* le paredes */
	abrirParedes(arq);
}

/******************************************************************************/
