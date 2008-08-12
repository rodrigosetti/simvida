#include <QtGui>
#include "opcoes.h"

// if we include <QtGui> there is no need to include every class used: <QString>, <QFileDialog>,...

Opcoes::Opcoes(QWidget *parent, Qt::WFlags f) : QWidget(parent, f)
{
    setupUi(this); // this sets up GUI
	setWindowFlags(f & (~Qt::WindowMaximizeButtonHint));

	/* Conecta botao */
	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(aceitou()));
	QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

/******************************************************************************/

void Opcoes::setMundo(Mundo *m)
{
	mundo = m;
}

/******************************************************************************/

void Opcoes::setBarras(QScrollBar *x, QScrollBar *y)
{
	barX = x;
	barY = y;
}

/******************************************************************************/

void Opcoes::showEvent ( QShowEvent * event )
{
	Q_UNUSED(event)

	/* Calibra spins */
	spinTeto->setValue(mundo->propriedades.teto_energetico);
	spinGrao->setValue(mundo->propriedades.energia_grao);
	spinProbabilidade->setValue(mundo->propriedades.probabilidade_mutacao);
	spinIntensidade->setValue(mundo->propriedades.intensidade_mutacao);
	spinHorizontal->setValue(mundo->propriedades.tamanho_x);
	spinVertical->setValue(mundo->propriedades.tamanho_y);
}

/******************************************************************************/

void Opcoes::aceitou()
{
	/* modifica propriedades do mundo */
	mundo->propriedades.teto_energetico = spinTeto->value();
	mundo->propriedades.energia_grao = spinGrao->value();
	mundo->propriedades.probabilidade_mutacao = spinProbabilidade->value();
	mundo->propriedades.intensidade_mutacao = spinIntensidade->value();
	mundo->propriedades.tamanho_x = spinHorizontal->value();
	mundo->propriedades.tamanho_y = spinVertical->value();

	/* muda barras */
	barX->setRange(0, mundo->propriedades.tamanho_x);
	barY->setRange(0, mundo->propriedades.tamanho_y);

	if (checkPadrao->isChecked())
	{
		/* Tornar padrao */
		/* Cria arquivo e escreve valores */
		FILE *arq = fopen("simvida.cfg", "w");

		fprintf(arq, "%d\n%d\n%d\n%d\n%d\n%d\n",
		mundo->propriedades.tamanho_x,
		mundo->propriedades.tamanho_y,
		mundo->propriedades.energia_grao,
		mundo->propriedades.teto_energetico,
		mundo->propriedades.probabilidade_mutacao,
		mundo->propriedades.intensidade_mutacao);

		/* Fecha arquivo */
		fclose(arq);
	}

	close();
}

/******************************************************************************/
