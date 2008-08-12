#include "mundo.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>


/* Constates para valores padroes */
#define TAMANHO_X 8192
#define TAMANHO_Y 6144

#define ENERGIA_GRAO 200
#define TETO_ENERGETICO ((TAMANHO_X*TAMANHO_Y)/800)

#define PROBABILIDADE_MUTACAO 90
#define INTENSIDADE_MUTACAO 1

/* Classe herdeira de Mundo */
class MundoFast : public Mundo
{
public:

	void salvarPrimeiroBiota(FILE *arq)
	{
		nohCabecaBiotas.proximo->biota.salvar(arq);
	}

};


/* MAIN */
int main(int argc, char **argv)
{
	/* Cria instancia do mundo simvida */
	MundoFast *mundo = new MundoFast();

	/* Verifica se existe arquivo com configuracoes */
	FILE *arq = fopen("simvida.cfg", "r");

	if (arq == NULL)
	{
		/* Nao existe arquivo : coloca valores padrao */
		mundo->propriedades.tamanho_x = TAMANHO_X;
		mundo->propriedades.tamanho_y =  TAMANHO_Y;
		mundo->propriedades.energia_grao = ENERGIA_GRAO;
		mundo->propriedades.teto_energetico = TETO_ENERGETICO;
		mundo->propriedades.probabilidade_mutacao = PROBABILIDADE_MUTACAO;
		mundo->propriedades.intensidade_mutacao = INTENSIDADE_MUTACAO;
		/* Cria arquivo e escreve valores padrao */
		arq = fopen("simvida.cfg", "w");
		fprintf(arq, "%d\n%d\n%d\n%d\n%d\n%d\n",
		TAMANHO_X, TAMANHO_Y, ENERGIA_GRAO,
		TETO_ENERGETICO, PROBABILIDADE_MUTACAO, INTENSIDADE_MUTACAO);
	}
	else
	{
		/* Carrega configuracoes de arquivo */
		fscanf(arq, "%d\n%d\n%d\n%d\n%d\n%d\n",
		&(mundo->propriedades.tamanho_x), &(mundo->propriedades.tamanho_y),
		&(mundo->propriedades.energia_grao),
		&(mundo->propriedades.teto_energetico), &(mundo->propriedades.probabilidade_mutacao),
		&(mundo->propriedades.intensidade_mutacao));
	}
	/* Fecha arquivo */
	fclose(arq);

	/* Verifica se ha linha de comando */
	if (argc == 1)
	{
		int inicial = rand() % 2+((mundo->propriedades.tamanho_x*mundo->propriedades.tamanho_y)/250000);
		for (int c = 0; c < inicial; c++)
			mundo->inserirBiota(Biota(mundo));
	}
	else
	{
		/* Le e abre todos os biotas */
		FILE *arq;
		for (int c = 1; c < argc; c++)
		{
			arq = fopen(argv[c], "r");
			mundo->inserirBiota(
			Biota(mundo, Vetor(
			rand() % mundo->propriedades.tamanho_x,
			rand() % mundo->propriedades.tamanho_y),
			arq));
			fclose(arq);
		}
	}

	/* Solicita taxa de decaimento energetico */
	int taxa_decaimento, freq_decaimento;
	printf("Entre com a frequencia de decaimento energetico:\n");
	scanf("%d",&freq_decaimento);
	printf("Entre com a taxa de decaimento energetico:\n");
	scanf("%d",&taxa_decaimento);

	unsigned int max_teto = mundo->propriedades.teto_energetico;
	int percentagem = 0;

	/* Loop ate numero de biotas ser igual a 1 */
	printf("Simulando...\n");
	while (mundo->estatisticas.numero_biotas > 1)
	{
		if ((mundo->estatisticas.ciclos % freq_decaimento) == 0 &&
        ((signed int)mundo->propriedades.teto_energetico - taxa_decaimento > 0))
		{
            mundo->propriedades.teto_energetico -= taxa_decaimento;
			if (percentagem < (max_teto - mundo->propriedades.teto_energetico)*100/max_teto)
			{
				printf("%d%%\n", percentagem++);
			}
		}

		mundo->atualizar();
	}
	printf("Simulacao finalizada: Um biota sobrevivente(sobrevivente.biota)\n");
	printf("%d ciclos, com %d wens de teto energetico.\n",
	mundo->estatisticas.ciclos, mundo->propriedades.teto_energetico);

	/* Seleciona e salva biota */
	arq = fopen("sobrevivente.biota", "w");
	mundo->salvarPrimeiroBiota(arq);
	fclose(arq);

	delete mundo;

	system("PAUSE");
	return 0;
}
