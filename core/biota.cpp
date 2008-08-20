#include "biota.h"
#include "mundo.h"
#include <math.h>
#include <stdlib.h>

#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QStringList>

/******************************************************************************/

unsigned int current_lineage = 0;

/* Macro para pegar o modulo de um numero */
#define ABS(a) (((a) > 0)? (a) : -(a))

/* Macro para correcao de angulo */
#define CORRIGE(a) (((a) < 0)? (a)+DPI : ((a) >= DPI)? (a) - DPI : (a))
#define CORRIGE2(a) (((a) < -DPI)? (a)+DPI : ((a) > DPI)? (a) - DPI : (a))

/* Macro para a operacao logicao ou exclusivo */
#define XOR(a,b) (((a) && !(b)) || (!(a) && (b)))

/* Macro para verificar se C esta a esquerda do vetor AB */
#define ESQUERDA(a,b,c) (area_triangulo(a,b,c) > 0)

/* Media estatistica de estabilização mutativa de numero de segmentos */
#define MEDIA_SEGMENTOS 8

/******************************************************************************/

float area_triangulo(Vetor<float> A, Vetor<float> B, Vetor<float> C)
{
  return A.X * B.Y - A.Y * B.X +
	 A.Y * C.X - A.X * C.Y +
	 B.X * C.Y - C.X * B.Y;
}

/******************************************************************************/

/* Construtores */
/* Constroi biota nulo */
Biota::Biota()
{
	lineage = current_lineage++;

	mundo = NULL;
	numero_segmentos = 0;

	/* Zera estrutura de genes */
	genes.cor_cabeca[0] = 0;
	genes.cor_cabeca[1] = 0;
	genes.cor_cabeca[2] = 0;
	genes.distribuicao_energia= 0;
	genes.limiar_reproducao = 0;
	genes.massa_cabeca = 0;
	genes.segmentos = NULL;
	genes.angulo_reproducao = 0.0f;
	genes.angulo_colisao = 0.0f;

	/* Zera estrutura do estado */
	estado.energia = 0;
	estado.geracao = 1;
	estado.idade = 0;
	estado.posicao = Vetor<float>();
	estado.velocidade =  Vetor<float>();
	estado.velocidade_angular = 0.0f;
	estado.posicaoSegmentos = NULL;
	estado.angulo = 0.0f;
	estado.filhos = 0;
}

/******************************************************************************/

/* Constroi biota aleatorio */
Biota::Biota(void *mundoPai)
{
	lineage = current_lineage++;

	mundo = mundoPai;
	numero_segmentos = 1 + (rand() % MEDIA_SEGMENTOS);

	/* Cria genes aleatoriamente */
	genes.cor_cabeca[0] = rand() % 255;
	genes.cor_cabeca[1] = rand() % 255;
	genes.cor_cabeca[2] = rand() % 255;
	genes.distribuicao_energia= (10 + (rand() % 70)) / 100.0f;
	genes.limiar_reproducao = ((Mundo*)mundo)->propriedades.energia_grao;
	genes.massa_cabeca = RAIO_GRAO + (rand() % 10);
	genes.angulo_reproducao = (((rand() % 1000) * DPI) / 1000.0f) - PI;
	genes.angulo_colisao = (((rand() % 1000) * MPI) / 200000.0f) - (QPI/20.0f);

	genes.segmentos = new struct GenesSegmento[numero_segmentos];
	for (unsigned int c = 0; c < numero_segmentos; c++)
	{
		genes.segmentos[c].arco = ((rand() % 1000)*QPI)/1000.0f;
		genes.segmentos[c].massa = 2 + (rand() % 12);
		genes.segmentos[c].comprimento = 10 + (rand() % 20);
		genes.segmentos[c].angulo = (QPI/2.0f) + (((rand() % 1000)*(PI+QPI))/1000.0f);
		genes.segmentos[c].cor[0] = rand() % 256;
		genes.segmentos[c].cor[1] = rand() % 256;
		genes.segmentos[c].cor[2] = rand() % 256;
		genes.segmentos[c].fa = 5 + (rand() % 95);
		genes.segmentos[c].fb = 5 + (rand() % 95);
	}

	/* Seta estado inicial padrao */
	estado.energia = genes.limiar_reproducao;
	estado.geracao = 1;
	estado.idade = 0;
	estado.posicao =
	Vetor<float>(rand() % ((Mundo*)mundo)->propriedades.tamanho_x,
	rand() % ((Mundo*)mundo)->propriedades.tamanho_y);
	estado.angulo = ((rand() % 1000) * DPI) / 1000.0f;
	estado.velocidade = Vetor<float>();
	estado.velocidade_angular = 0.0f;
	estado.filhos = 0;
	estado.posicaoSegmentos = new float[numero_segmentos];
	for (unsigned int c = 0; c < numero_segmentos; c++)
	{
		estado.posicaoSegmentos[c] = 0.0f;
	}
}

/******************************************************************************/

/* Constroi biota baseado em outro biota */
Biota::Biota(const Biota &copia)
{
	lineage = copia.lineage;

	/* Copia genes e estado */
	genes = copia.genes;
	estado = copia.estado;

	/* Copia numero de segmentos */
	numero_segmentos = copia.numero_segmentos;
	mundo = copia.mundo;

	/* Copia vetor de genes e estados */
	estado.posicaoSegmentos = new float[numero_segmentos];
	for (unsigned int c = 0; c < numero_segmentos; c++)
	{
		estado.posicaoSegmentos[c] = 0.0f;
	}

	genes.segmentos = new struct GenesSegmento[numero_segmentos];
	for (unsigned int c = 0; c < numero_segmentos; c++)
		genes.segmentos[c] = copia.genes.segmentos[c];
}

/******************************************************************************/

/* Constroi biota no mundo e posicao dados com base em arquivo */
Biota::Biota(void *mundoPai, Vetor<float> posicao, FILE *arquivo)
{
	Biota((void*)mundoPai);

	lineage = current_lineage++;
	abrir(arquivo);

	estado.posicao = posicao;
}

/******************************************************************************/

/* Destructor */
void Biota::destroy()
{
	/* Libera memoria */
	delete genes.segmentos;
	delete estado.posicaoSegmentos;
}

/******************************************************************************/

/* Metodos */
/* Atualiza biota e retorna estado */
enum ResultadoAtualizacao Biota::atualizar(void *noh)
{
	/* massa total */
	unsigned int massa_total = genes.massa_cabeca;

	/* Forca de movimentacao */
	Vetor<float> forca;

	/* Aceleracao angular */
	float aceleracao_angular = 0.0f;

	/* Atualiza estado do biota */
	for (unsigned int c = 0; c < numero_segmentos; c++)
	{
		float ang, vvetor;

		/* move segmento de acordo com a força */
		if (cos(estado.posicaoSegmentos[c]) < 0)
			estado.posicaoSegmentos[c] +=
			((genes.segmentos[c].fa / genes.segmentos[c].massa) / genes.segmentos[c].comprimento) *
			VEL_SEGMENTO;
		else
			estado.posicaoSegmentos[c] +=
			((genes.segmentos[c].fb / genes.segmentos[c].massa) / genes.segmentos[c].comprimento) *
			VEL_SEGMENTO;

		/* Corrige angulo */
		estado.posicaoSegmentos[c] = CORRIGE(estado.posicaoSegmentos[c]);

		/* Calcula todo resto */
		if (cos(estado.posicaoSegmentos[c]) < 0)
		{

			/* Calcula forca da remada do segmento */
			vvetor = ((genes.segmentos[c].massa * genes.segmentos[c].fa*
			genes.segmentos[c].comprimento) * FLUIDEZ) * genes.segmentos[c].arco
			* (1.5 - ABS(sin(estado.posicaoSegmentos[c])));

			/* Calcula angulo da remada do segmento */
			ang = PI+estado.angulo+genes.segmentos[c].angulo+((genes.segmentos[c].fa * FREIO_IMPULSO)*
			genes.segmentos[c].arco)+(sin(estado.posicaoSegmentos[c]) * (genes.segmentos[c].arco / 2.0f));

			/* Acrecenta velocidade angular */
			aceleracao_angular += vvetor;

			/* Perde energia */
			estado.energia -= genes.segmentos[c].fa * vvetor * DISSIPACAO_ENERGETICA;
		}
		else
		{
			/* Calcula forca da remada do segmento */
			vvetor = ((genes.segmentos[c].massa * genes.segmentos[c].fb*
			genes.segmentos[c].comprimento) * FLUIDEZ) * genes.segmentos[c].arco
			* (1.5 - ABS(sin(estado.posicaoSegmentos[c])));

			/* Calcula angulo da remada do segmento */
			ang = PI+estado.angulo+genes.segmentos[c].angulo+((genes.segmentos[c].fb * FREIO_IMPULSO)*
			genes.segmentos[c].arco)+(sin(estado.posicaoSegmentos[c]) * (genes.segmentos[c].arco / 2.0f));

			/* Acrecenta velocidade angular */
			aceleracao_angular -= vvetor;

			/* Perde energia */
			estado.energia -= genes.segmentos[c].fb * vvetor * DISSIPACAO_ENERGETICA;
		}

		/* Soma a massa total */
		massa_total += genes.segmentos[c].massa;

		/* Soma s velocidade a aceleracao */
		forca += Vetor<float>(cos(ang), sin(ang)) * vvetor;
	}

	/* Atualiza velocidade */
	estado.velocidade += forca / massa_total;
	estado.velocidade_angular += aceleracao_angular / (VISCOSIDADE * massa_total);

	/* Atualiza posicao */
	estado.posicao += estado.velocidade;

	/* Atualiza angulo */
	estado.angulo += estado.velocidade_angular;

	/* Aplica atrito as velocidades */
	estado.velocidade *= ATRITO;
	estado.velocidade_angular *= ATRITO_ANGULAR;

	/* Incrementa idade do biota */
	estado.idade++;

	/* Perde energia de acordo com idade */
	estado.energia -= PERDA_ENERGETICA_IDADE * estado.idade;

	/**********************************************************************/
	/* Trata colisoes com os limites */
	/**********************************************************************/

	if (estado.posicao.X < (signed int)(genes.massa_cabeca + GROSSURA_PAREDE))
	{
		estado.posicao.X = genes.massa_cabeca + GROSSURA_PAREDE;
		estado.velocidade.X *= -1.0f;

		estado.velocidade_angular += genes.angulo_colisao;
	}
	else if (estado.posicao.X > (signed int)(((Mundo*)mundo)->propriedades.tamanho_x - genes.massa_cabeca - GROSSURA_PAREDE))
	{
		estado.posicao.X = ((Mundo*)mundo)->propriedades.tamanho_x - genes.massa_cabeca - GROSSURA_PAREDE;
		estado.velocidade.X *= -1.0f;

		estado.velocidade_angular += genes.angulo_colisao;
	}
	if (estado.posicao.Y < (signed int)(genes.massa_cabeca + GROSSURA_PAREDE))
	{
		estado.posicao.Y = genes.massa_cabeca + GROSSURA_PAREDE;
		estado.velocidade.Y *= -1.0f;

		estado.velocidade_angular += genes.angulo_colisao;
	}
	else if (estado.posicao.Y > (signed int)(((Mundo*)mundo)->propriedades.tamanho_y - genes.massa_cabeca - GROSSURA_PAREDE))
	{
		estado.posicao.Y = ((Mundo*)mundo)->propriedades.tamanho_y - genes.massa_cabeca - GROSSURA_PAREDE;
		estado.velocidade.Y *= -1.0f;

		estado.velocidade_angular += genes.angulo_colisao;
	}

	/**********************************************************************/
	/* Trata colisao com as paredes */
	/**********************************************************************/

	/* Percorre lista de paredes */
	for (struct NohParede *percorre = ((Mundo*)mundo)->nohCabecaParedes.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		Vetor<float> u = estado.posicao - percorre->inicio;
		Vetor<float> v = percorre->fim - percorre->inicio;
		Vetor<float> proj = (v.modulo() == 0)? estado.posicao : (
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
		if (estado.posicao.distancia(proj) <=
		genes.massa_cabeca + GROSSURA_PAREDE)
		{
			/* Colisao */
			/* Calcula angulo de colisao */
			float ang = (estado.posicao.Y - proj.Y == 0 && estado.posicao.X - proj.X == 0)? 0.0f :
			atan2(estado.posicao.Y - proj.Y, estado.posicao.X - proj.X);

			/* Reajusta posicao */
			estado.posicao += Vetor<float>(cos(ang),sin(ang)) *
			((genes.massa_cabeca + GROSSURA_PAREDE) - estado.posicao.distancia(proj));

			/* Reajusta velocidade */
			float dist = estado.posicao.distancia(proj);
			Vetor<float> acel = (estado.posicao - proj) / dist;

			u = (estado.posicao+estado.velocidade) - percorre->inicio;
			v = percorre->fim - percorre->inicio;
			Vetor<float> proj = (v.modulo() == 0)? estado.posicao : (
			(v / v.modulo ()) *
			(((u.X * v.X)+(u.Y * v.Y)) / v.modulo()) );
			proj += percorre->inicio;

			acel *=	(dist - (estado.posicao+estado.velocidade).distancia(proj))*2.0f;
			estado.velocidade += acel;

			/* Muda angulo */
			estado.velocidade_angular += genes.angulo_colisao;
		}
	}

	/**********************************************************************/
	/* Trata colisao com outros biotas */
	/**********************************************************************/

	/* Percorre noh a partir do atual */
	for (struct Mundo::NohBiota *percorre = ((Mundo::NohBiota*)noh)->proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		/* calcula distancia */
		float distancia = estado.posicao.distancia(percorre->biota.estado.posicao);

		/* Verifica se distancia eh colisiva */
		if (distancia <= genes.massa_cabeca + percorre->biota.genes.massa_cabeca)
		{
			/* Modifica posicao */
			estado.posicao -= (estado.posicao - percorre->biota.estado.posicao).reduzir() *
			((distancia - (genes.massa_cabeca + percorre->biota.genes.massa_cabeca))/2.0f);
			percorre->biota.estado.posicao += (estado.posicao - percorre->biota.estado.posicao).reduzir() *
			((distancia - (genes.massa_cabeca + percorre->biota.genes.massa_cabeca))/2.0f);

			/* Modifica velocidade */
			if (distancia > 0.0f)
			{
				Vetor<float> tA, tB;

				if (estado.velocidade.Y != 0 || estado.velocidade.X != 0)
					tA = (estado.posicao - percorre->biota.estado.posicao).reduzir() *
				       (cos(atan2(estado.posicao.Y - percorre->biota.estado.posicao.Y,
					estado.posicao.X - percorre->biota.estado.posicao.X) -
					atan2(estado.velocidade.Y, estado.velocidade.X)) *
					sqrt((estado.velocidade.X*estado.velocidade.X)+(estado.velocidade.Y*estado.velocidade.Y)));

				if (percorre->biota.estado.velocidade.Y != 0 || percorre->biota.estado.velocidade.X != 0)
					tB = (estado.posicao - percorre->biota.estado.posicao).reduzir() *
				       (cos(atan2(estado.posicao.Y - percorre->biota.estado.posicao.Y,
					estado.posicao.X - percorre->biota.estado.posicao.X) -
					atan2(percorre->biota.estado.velocidade.Y, percorre->biota.estado.velocidade.X)) *
					sqrt((percorre->biota.estado.velocidade.X*percorre->biota.estado.velocidade.X)+
					(percorre->biota.estado.velocidade.Y*percorre->biota.estado.velocidade.Y)));


				/* Aplica formulas de quantidade de movimento */
				tA *= genes.massa_cabeca/percorre->biota.genes.massa_cabeca;
				tB *= percorre->biota.genes.massa_cabeca/genes.massa_cabeca;

				/* Transfere energias */
				estado.velocidade -= tA * 1.5f;
				percorre->biota.estado.velocidade -= tB * 1.5f;

				percorre->biota.estado.velocidade += tA * 1.5f;
				estado.velocidade += tB * 1.5f;
			}

			/* Troca velocidades angulares */
			float troca;
			troca = estado.velocidade_angular;
			estado.velocidade_angular = -percorre->biota.estado.velocidade_angular;
			percorre->biota.estado.velocidade_angular = -troca;

		}
	}


	/**********************************************************************/
	/* Verifica colisao com grao de comida ou perseguicao a grao de comida*/
	/**********************************************************************/
	unsigned int menor_distancia = ((Mundo*)mundo)->propriedades.tamanho_x;
	float angulo_grao = 0.0f;

	/* Percorre graos de comida */
	struct NohGrao *percorre = &(((Mundo*)mundo)->nohCabecaGraos);
	while(percorre->proximo != NULL)
	{
		unsigned int distancia = (unsigned int)percorre->proximo->posicao.distancia(estado.posicao);
		/* Se tocou no grao */
		if (distancia <= (unsigned int)(genes.massa_cabeca + RAIO_GRAO))
		{
			/* Remove grao especifico da lista */
			struct NohGrao *remover = percorre->proximo;
			percorre->proximo = percorre->proximo->proximo;

			/* Corrige caso seja final da lista */
			if (remover == ((Mundo*)mundo)->finalGraos)
				((Mundo*)mundo)->finalGraos = percorre;

			if (remover == ((Mundo*)mundo)->grao_selecionado)
				((Mundo*)mundo)->grao_selecionado = NULL;

			delete remover;
			((Mundo*)mundo)->estatisticas.numero_graos--;

			/* Acrescenta energia */
			estado.energia += ((Mundo*)mundo)->propriedades.energia_grao;
			break;
		}
		/* Se este eh o mais proximo encontrado, esta a vista e nao houve colisao */
		else if (distancia <= genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS &&
		distancia < menor_distancia)
		{
			/* Verifica se nao ha nenhuma parede a vista */
			bool grao_visivel = true;
			/* Percorre lista de paredes */
			for (struct NohParede *percorre_par = ((Mundo*)mundo)->nohCabecaParedes.proximo;
			percorre_par != NULL;
			percorre_par = percorre_par->proximo)
				if (XOR(ESQUERDA(percorre_par->inicio,percorre_par->fim,percorre->proximo->posicao),
				 ESQUERDA(percorre_par->inicio,percorre_par->fim,estado.posicao)) &&
				 XOR(ESQUERDA(percorre->proximo->posicao,estado.posicao,percorre_par->inicio),
				 ESQUERDA(percorre->proximo->posicao,estado.posicao,percorre_par->fim)))
				{
					/* Intersecccao ! */
					grao_visivel = false;
					break;
				}

			if (grao_visivel)
			{
				/* Este eh o grao mais proximo */
				menor_distancia = distancia;
				angulo_grao = atan2(percorre->proximo->posicao.Y - estado.posicao.Y,
				percorre->proximo->posicao.X - estado.posicao.X);
			}
		}
		percorre = percorre->proximo;
	}

	/* Efetua correcao do angulo */
	estado.angulo = CORRIGE(estado.angulo);

	/* Se encontrou um grao proximo */
	if (menor_distancia < ((Mundo*)mundo)->propriedades.tamanho_x)
	{
		/* Efetua correcao do angulo */
		angulo_grao = CORRIGE(angulo_grao);

		/* Gira cabeca em direcao ao grao localizado */
		if (ABS(estado.angulo - angulo_grao) < PI)
		{
			if (ABS(estado.angulo - angulo_grao) < ACEL_GIRO)
				estado.velocidade_angular -= estado.angulo - angulo_grao;
			else if (estado.angulo > angulo_grao)
				estado.velocidade_angular -= ACEL_GIRO;
			else
				estado.velocidade_angular += ACEL_GIRO;
		}
		else if (estado.angulo > angulo_grao)
		{
			if ((DPI - estado.angulo) + angulo_grao <= ACEL_GIRO)
				estado.velocidade_angular +=  (DPI - estado.angulo) + angulo_grao;
			else
				estado.velocidade_angular += ACEL_GIRO;
		}
		else
		{
			if ((DPI - angulo_grao) +  estado.angulo <= ACEL_GIRO)
				estado.velocidade_angular -=  (DPI - angulo_grao) +  estado.angulo;
			else
				estado.velocidade_angular -= ACEL_GIRO;
		}

		/* Retira energia gasta por isso */
		estado.energia -= (genes.massa_cabeca * (estado.velocidade_angular/QPI)) * PERDA_ENERGIA_CABECA;
	}

	/* Corrige angulo */
	estado.velocidade_angular = CORRIGE2(estado.velocidade_angular);

	/* Retorna estado */
	return (estado.energia < 0.0f)? MORTE :
	((estado.energia > genes.limiar_reproducao)? REPRODUCAO : NORMAL);
}

/******************************************************************************/

/* Aplica mutacao com base na intensidade */
void Biota::mutacao()
{
	#define CABECA	-1

	for (unsigned int x = 0; x < ((Mundo*)mundo)->propriedades.intensidade_mutacao; x++)
	{
		/* flag de mutacao para cor */
		int mutacao = CABECA; /* Mutacao na cabeca */
		int tendencia = (rand() % 2 == 0)? -1 : 1;
		int indice = rand() % (7 + numero_segmentos);

		switch (indice)
		{
			case 0:
				genes.massa_cabeca += tendencia;
				if (genes.massa_cabeca <= 0)
					genes.massa_cabeca = 1;
				break;
			case 1:
				genes.limiar_reproducao += tendencia;
				if (genes.limiar_reproducao <= 0)
					genes.limiar_reproducao = 1;
				break;
			case 2:
				genes.distribuicao_energia += tendencia / 100.0f;
				if (genes.distribuicao_energia < 0.01f)
					genes.distribuicao_energia = 0.01f;
				else if (genes.distribuicao_energia > 1.0f)
					genes.distribuicao_energia = 1.0f;
				break;
			case 3:
				/* Decide se vai ganhar ou perder um segmento */
				if (rand() % 1000 > (numero_segmentos-1) * 500.0f / MEDIA_SEGMENTOS)
				{
					/* Ganha segmento */
					struct GenesSegmento *novosSegmentos =
					new struct GenesSegmento[numero_segmentos+1];
					float *novosEstados = new float[numero_segmentos+1];

					for (int s = 0; (unsigned int)s < numero_segmentos; s++)
					{
						novosSegmentos[s] = genes.segmentos[s];
						novosEstados[s] = 0.0f;
					}

					novosSegmentos[numero_segmentos].arco = ((rand() % 1000)*(QPI/2.0f))/1000.0f;
					novosSegmentos[numero_segmentos].massa = 1 + (rand() % 11);
					novosSegmentos[numero_segmentos].comprimento = 10 + (rand() % 20);
					novosSegmentos[numero_segmentos].angulo = (QPI/2.0f) + (((rand() % 1000)*(PI+QPI))/1000.0f);
					novosSegmentos[numero_segmentos].cor[0] = rand() % 255;
					novosSegmentos[numero_segmentos].cor[1] = rand() % 255;
					novosSegmentos[numero_segmentos].cor[2] = rand() % 255;
					novosSegmentos[numero_segmentos].fa = 20 + (rand() % 100);
					novosSegmentos[numero_segmentos].fb = 20 + (rand() % 100);

					numero_segmentos++;

					delete genes.segmentos;
					genes.segmentos = novosSegmentos;

					delete estado.posicaoSegmentos;
					estado.posicaoSegmentos = novosEstados;
				}
				else
				{
					/* Perde segmento */
					if (numero_segmentos == 1)
						break;
					struct GenesSegmento *novosSegmentos =
					new struct GenesSegmento[numero_segmentos-1];
					float *novosEstados = new float[numero_segmentos-1];
					unsigned int remover = rand() % numero_segmentos;

					for (unsigned int s = 0, n = 0; s < numero_segmentos; s++)
						if (s != remover)
						{
							novosEstados[n] = 0.0f;
							novosSegmentos[n] = genes.segmentos[s];
							n++;
						}

					numero_segmentos--;

					delete genes.segmentos;
					genes.segmentos = novosSegmentos;

					delete estado.posicaoSegmentos;
					estado.posicaoSegmentos = novosEstados;
				}
				break;
			case 5:
				genes.angulo_reproducao += tendencia / 10.0f;
				genes.angulo_reproducao = CORRIGE2(genes.angulo_reproducao);
				break;

			case 6:
				genes.angulo_colisao += tendencia / 10.0f;
				genes.angulo_colisao = CORRIGE2(genes.angulo_colisao);
				break;
			default:
				unsigned int c = rand() % numero_segmentos;
				mutacao = c; /* Mutacao no segmento dado */
				switch (rand() % 6)
				{
					case 0:
						genes.segmentos[c].arco += tendencia / 10.0f;
						if (genes.segmentos[c].arco < 0.0f)
							genes.segmentos[c].arco = 0.0f;
						break;
					case 1:
						genes.segmentos[c].comprimento += tendencia;
						if (genes.segmentos[c].comprimento <= 0)
							genes.segmentos[c].comprimento = 1;
						break;
					case 2:
						genes.segmentos[c].massa += tendencia;
						if (genes.segmentos[c].massa <= 0)
							genes.segmentos[c].massa = 1;
						break;
					case 3:
						genes.segmentos[c].angulo += tendencia / 10.0f;
						if (genes.segmentos[c].angulo < 0.0f)
							genes.segmentos[c].angulo = 0.0f;
						break;
					case 4:
						genes.segmentos[c].fa += tendencia / 10.0f;
						if (genes.segmentos[c].fa < 0.0f)
							genes.segmentos[c].fa = 0.0f;
						break;
					case 5:
						genes.segmentos[c].fb += tendencia / 10.0f;
						if (genes.segmentos[c].fb < 0.0f)
							genes.segmentos[c].fb = 0.0f;
						break;
				}
			break;
		}

		#define TAXA_COR 5

		indice %= 3;

		/* Muda cor */
		if (mutacao == CABECA)
		{
			if (genes.cor_cabeca[indice] > TAXA_COR && tendencia < 0)
				genes.cor_cabeca[indice] -= TAXA_COR;
			else if (genes.cor_cabeca[indice] < 255-TAXA_COR)
				genes.cor_cabeca[indice] += TAXA_COR;
		}
		else
		{
			if (genes.segmentos[mutacao].cor[indice] > TAXA_COR && tendencia < 0)
				genes.segmentos[mutacao].cor[indice] -= TAXA_COR;
			else if (genes.segmentos[mutacao].cor[indice] < 255-TAXA_COR)
				genes.segmentos[mutacao].cor[indice] += TAXA_COR;
		}

	}
}

/******************************************************************************/

/* Verifica se biota eh selecionavel na posicao dada */
bool Biota::selecionar(Vetor<float> posicao)
{
	return (posicao.distancia(estado.posicao) <= genes.massa_cabeca * FATOR_SELECAO);
}

/******************************************************************************/

/* Salva genes do biota no arquivo dado */
void Biota::salvar(FILE* arquivo, bool saveState, int ident)
{
	if (ident == 0)
	{
		fprintf(arquivo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf(arquivo, "<!DOCTYPE biot SYSTEM \"http://simvida.sourceforge.net/simvida.biot.dtd\">\n");
	}

	char *tab = new char[ident+1];
	for (int c = 0; c < ident; c++) tab[c] = '\t';
	tab[ident] = '\0';

	fprintf(arquivo, "%s<biot headcolor=\"%d;%d;%d\" headmass=\"%d\" reproductionthreshold=\"%d\"\n", tab,
		genes.cor_cabeca[0],genes.cor_cabeca[1],genes.cor_cabeca[2], genes.massa_cabeca, genes.limiar_reproducao);
	fprintf(arquivo, "%s      reproductionangle=\"%.4f\" energydistribution=\"%.4f\" colisionturnangle=\"%.4f\"", tab,
		genes.angulo_reproducao, genes.distribuicao_energia, genes.angulo_colisao);

	if (saveState)
	{
		fprintf(arquivo, "\n%s      position=\"%d;%d\" velocity=\"%.2f;%.2f\" angularvelocity=\"%.4f\" anglecicle=\"%.4f\"\n", tab,
		(int)estado.posicao.X, (int)estado.posicao.Y, estado.velocidade.X, estado.velocidade.Y, estado.velocidade_angular, estado.angulo);
		fprintf(arquivo, "%s      energy=\"%.4f\" age=\"%d\" generation=\"%d\" offsprings=\"%d\"", tab,
		estado.energia, estado.idade, estado.geracao, estado.filhos);
	}
	fprintf(arquivo, ">\n");

	for (unsigned int c = 0; c < numero_segmentos; c++)
	{
		fprintf(arquivo, "%s\t<segment color=\"%d;%d;%d\" mass=\"%d\" angle=\"%.4f\" length=\"%d\" arcangle=\"%.4f\" forces=\"%.4f;%.4f\"", tab,
			genes.segmentos[c].cor[0],genes.segmentos[c].cor[1],genes.segmentos[c].cor[2],
			genes.segmentos[c].massa, genes.segmentos[c].angulo, genes.segmentos[c].comprimento,
			genes.segmentos[c].arco, genes.segmentos[c].fa, genes.segmentos[c].fb);
		if (saveState)
		{
			fprintf(arquivo, " angleposition=\"%.4f\"", estado.posicaoSegmentos[c]);
		}
		fprintf(arquivo, "/>\n");
	}
	fprintf(arquivo, "%s</biot>\n", tab);
}

/******************************************************************************/

/* Carrega genes do arquivo */
void Biota::abrir(FILE* arquivo)
{
	QDomDocument xmlDocument;
	QFile file;
	file.open(arquivo, QIODevice::ReadOnly);
	xmlDocument.setContent(&file);
	file.close();

	QDomNodeList children = xmlDocument.childNodes();
	for (int i=0; i < children.count(); i++)
	{
		/* try to convert the node to an element. */
		QDomElement element = children.at(i).toElement();
		if (!element.isNull() && element.tagName() == "biot")
		{
			abrir(element);
			break;
		}
	}

	xmlDocument.clear();

	/* Seta estado inicial */
	estado.energia = genes.limiar_reproducao-1;
}

/******************************************************************************/

void Biota::abrir(QDomNode xmlNode)
{
	/* try to convert the node to an element. */
	QDomElement element = xmlNode.toElement();
	if (!element.isNull() && element.tagName() == "biot")
	{
		QStringList headcolor = element.attribute("headcolor").split(";");
		genes.cor_cabeca[0] = headcolor.at(0).toInt();
		genes.cor_cabeca[1] = headcolor.at(1).toInt();
		genes.cor_cabeca[2] = headcolor.at(2).toInt();

		genes.massa_cabeca = element.attribute("headmass").toInt();
		genes.limiar_reproducao = element.attribute("reproductionthreshold").toInt();
		genes.distribuicao_energia = element.attribute("energydistribution").toFloat();
		genes.angulo_reproducao = element.attribute("reproductionangle").toFloat();
		genes.angulo_colisao = element.attribute("colisionturnangle").toFloat();

		QStringList position = element.attribute("position", "0;0").split(";");
		estado.posicao.X = position.at(0).toFloat();
		estado.posicao.Y = position.at(1).toFloat();

		QStringList velocity = element.attribute("velocity", "0;0").split(";");
		estado.velocidade.X = velocity.at(0).toFloat();
		estado.velocidade.Y = velocity.at(1).toFloat();

		estado.velocidade_angular = element.attribute("angularvelocity", "0").toFloat();
		estado.angulo = element.attribute("anglecicle", "0").toFloat();
		estado.geracao = element.attribute("generation", "1").toInt();
		estado.filhos = element.attribute("offsprings", "0").toInt();
		estado.idade = element.attribute("age", "0").toInt();
		estado.energia = element.attribute("energy", QString::number(genes.limiar_reproducao-1)).toFloat();

		QDomNodeList biotChildren = element.childNodes();
		numero_segmentos = biotChildren.count();

		genes.segmentos = new struct GenesSegmento[numero_segmentos];
		estado.posicaoSegmentos = new float[numero_segmentos];

		for (int j=0; j < biotChildren.count(); j++)
		{
			/* try to convert the node to an element. */
			QDomElement segment = biotChildren.at(j).toElement();
			if (!segment.isNull() && segment.tagName() == "segment")
			{
				genes.segmentos[j].arco = segment.attribute("arcangle").toFloat();
				genes.segmentos[j].comprimento = segment.attribute("length").toInt();
				genes.segmentos[j].massa = segment.attribute("mass").toInt();
				genes.segmentos[j].angulo = segment.attribute("angle").toFloat();

				estado.posicaoSegmentos[j] = segment.attribute("angleposition").toFloat();

				QStringList segcolor = segment.attribute("color").split(";");
				genes.segmentos[j].cor[0] = segcolor.at(0).toInt();
				genes.segmentos[j].cor[1] = segcolor.at(1).toInt();
				genes.segmentos[j].cor[2] = segcolor.at(2).toInt();

				QStringList forces = segment.attribute("forces").split(";");
				genes.segmentos[j].fa = forces.at(0).toFloat();
				genes.segmentos[j].fb = forces.at(1).toFloat();
			}
		}
	}
}

/******************************************************************************/

/* Coloca biota na posicao dada */
void Biota::posicionar(Vetor<float> posicao)
{
	estado.posicao = posicao;
}

/******************************************************************************/

/* Acelera biota */
void Biota::acelerar(Vetor<float> aceleracao)
{
	estado.velocidade += aceleracao;
}

/******************************************************************************/

/* Reflete anatomia */
void Biota::refletir()
{
	genes.angulo_reproducao = -genes.angulo_reproducao;
	genes.angulo_colisao = -genes.angulo_colisao;

	for (unsigned int c = 0; c < numero_segmentos; c++)
	{
		genes.segmentos[c].angulo = CORRIGE(-genes.segmentos[c].angulo);

		float troca;
		troca = genes.segmentos[c].fa;
		genes.segmentos[c].fa = genes.segmentos[c].fb;
		genes.segmentos[c].fb = troca;

		estado.posicaoSegmentos[c] += PI;
	}
}

/******************************************************************************/
