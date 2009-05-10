/** \file
    Implementacao da classe Mundo
    \author Rodrigo Setti
*/
#include "mundo.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <QStringList>
#include <QFile>

/******************************************************************************/

/* Operacoes com graos */
/** Acrescenta um novo grao na posicao dada */
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

/** Remove um grao da lista */
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

/** Construtor de um mundo vazio */
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

/** Constroi mundo com tamanho definido */
Mundo::Mundo(unsigned int tx, unsigned int ty)
{
	Mundo();
	propriedades.tamanho_x = tx;
	propriedades.tamanho_y = ty;
}

/******************************************************************************/

/** Constroi mundo com determinado numero de biotas aleatorios */
Mundo::Mundo(unsigned int numero_de_biotas)
{
	Mundo();
	for (unsigned int c = 0; c < numero_de_biotas; c++)
	{
		inserirBiota(Biota(this));
	}
}

/******************************************************************************/

/** Constroi mundo com tamanho definido e determinado numero de biotas */
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

/** Destrutor : limpa biotas e graos*/
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
	while (removerGrao())
        ;

	/* Zera estatisticas */
	estatisticas.numero_biotas = 0;
	estatisticas.numero_graos = 0;
	estatisticas.ciclos = 0;
	estatisticas.mortes = 0;
	estatisticas.nascimentos = 0;
}

/******************************************************************************/

/** Insere biota no mundo */
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

/** Atualiza mundo */
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

/** Seleciona biota na posicao, se existir */
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

/** Desceleciona biota se selecionado */
void Mundo::descelecionarBiota()
{
	selecionado = NULL;
}

/******************************************************************************/
/* OPERACOES COM GRAOS */
/******************************************************************************/

/** Remove grao selecionado */
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

/** Desceleciona grao se selecionado */
void Mundo::descelecionarGrao()
{
	grao_selecionado = NULL;
}

/******************************************************************************/

/** Seleciona grao na posicao, se existir */
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

/** Muda posicao do grao selecionado */
void Mundo::posicionarGrao(Vetor<float> posicao)
{
	if (grao_selecionado != NULL)
	{
		grao_selecionado->posicao = posicao;
	}
}

/******************************************************************************/

/** Retorna verdadeiro se existe grao selecionado */
bool Mundo::graoSelecionado()
{
	return (grao_selecionado != NULL);
}

/******************************************************************************/

/** Salva estado do mundo em um arquivo */
void Mundo::salvarMundo(FILE *arq)
{
	fprintf(arq, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(arq, "<!DOCTYPE simulation SYSTEM \"http://simvida.sourceforge.net/simvida.simulation.dtd\">\n");
	fprintf(arq, "<simulation cicles=\"%ld\" borns=\"%ld\" deaths=\"%ld\"\n",
		estatisticas.ciclos, estatisticas.nascimentos, estatisticas.mortes);

	/* salva propriedades */
	fprintf(arq, " energyceil=\"%d\" grainenergy=\"%d\" mutationprobability=\"%d\"\n",
		propriedades.teto_energetico, propriedades.energia_grao, propriedades.probabilidade_mutacao);
	fprintf(arq, " mutationintensity=\"%d\" size=\"%d;%d\">\n",
		propriedades.intensidade_mutacao, propriedades.tamanho_x, propriedades.tamanho_y);

	/* Salvar biotas */
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		/* salva biota */
		percorre->biota.salvar(arq, true, 1);
	}

	/* Salva graos */
	for (struct NohGrao *percorre = nohCabecaGraos.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		fprintf(arq, "\t<grain position=\"%d;%d\"/>\n", (int)percorre->posicao.X, (int)percorre->posicao.Y);
	}

	/* Salvar paredes */
	/* Percorre lista de paredes */
	for (struct NohParede *percorre = nohCabecaParedes.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		fprintf(arq, "\t<wall geometry=\"%d;%d;%d;%d\"/>\n",
			(int)percorre->inicio.X, (int)percorre->inicio.Y, (int)percorre->fim.X, (int)percorre->fim.Y);
	}

	/* separador */
	fprintf(arq, "</simulation>\n");
}

/******************************************************************************/

/** Abre mundo descrito em arquivo */
void Mundo::abrirMundo(FILE *arq)
{
	/* limpa mundo */
	destroy();

	QDomDocument xmlDocument;
	QFile file;
	file.open(arq, QIODevice::ReadOnly);
	if (!xmlDocument.setContent(&file))
		fprintf(stderr, "XML Parser error.\n");
	file.close();

	QDomNodeList children = xmlDocument.childNodes();
	for (int i=0; i < children.count(); i++)
	{
		/* try to convert the node to an element. */
		QDomElement element = children.at(i).toElement();
		if (!element.isNull() && element.tagName() == "simulation")
		{

			propriedades.teto_energetico = element.attribute("energyceil",
				QString::number(propriedades.teto_energetico)).toInt();
			propriedades.energia_grao = element.attribute("grainenergy",
				QString::number(propriedades.energia_grao)).toInt();
			propriedades.probabilidade_mutacao = element.attribute("mutationprobability",
				QString::number(propriedades.probabilidade_mutacao)).toInt();
			propriedades.intensidade_mutacao = element.attribute("mutationintensity",
				QString::number(propriedades.intensidade_mutacao)).toInt();

			QStringList size = element.attribute("size",
				QString::number(propriedades.tamanho_x) + ";" + QString::number(propriedades.tamanho_y)).split(";");
			propriedades.tamanho_x = size.at(0).toInt();
			propriedades.tamanho_y = size.at(1).toInt();

			estatisticas.ciclos = element.attribute("cicles", "0").toInt();
			estatisticas.mortes = element.attribute("deaths", "0").toInt();
			estatisticas.nascimentos = element.attribute("borns", "0").toInt();

			QDomNodeList simChildren = element.childNodes();
			for (int j=0; j < simChildren.count(); j++)
			{
				/* try to convert the node to an element. */
				QDomElement simElement = simChildren.at(j).toElement();
				if (!simElement.isNull() && simElement.tagName() == "biot")
				{
					Biota biota;
					biota.abrir(simElement);
					inserirBiota(biota);
				}
				else if (!simElement.isNull() && simElement.tagName() == "grain")
				{
					QStringList position = simElement.attribute("position").split(";");
					inserirGrao(Vetor<float>(position.at(0).toFloat(),position.at(1).toFloat()));
				}
				else if (!simElement.isNull() && simElement.tagName() == "wall")
				{
					QStringList geometry = simElement.attribute("geometry").split(";");
					int ax = geometry.at(0).toInt();
					int ay = geometry.at(1).toInt();
					int bx = geometry.at(2).toInt();
					int by = geometry.at(3).toInt();

					if (ax > -(signed int)propriedades.tamanho_x && ax < (signed int)propriedades.tamanho_x*2 &&
					bx > -(signed int)propriedades.tamanho_x && bx < (signed int)propriedades.tamanho_x*2 &&
					ay > -(signed int)propriedades.tamanho_y && ay < (signed int)propriedades.tamanho_y*2 &&
					ay > -(signed int)propriedades.tamanho_x && ay < (signed int)propriedades.tamanho_y*2)
					{
						/* Insere nova parede */
						struct NohParede *novaParede = new struct NohParede;

						novaParede->inicio = Vetor<float>(ax,ay);
						novaParede->fim = Vetor<float>(bx,by);
						novaParede->proximo = nohCabecaParedes.proximo;
						nohCabecaParedes.proximo = novaParede;
					}
				}
			}
		}
	}
	xmlDocument.clear();
}

/******************************************************************************/
