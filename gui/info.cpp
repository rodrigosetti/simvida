#include <QtGui>
#include "info.h"

/******************************************************************************/

Info::Info(QWidget *parent, Qt::WFlags f) : QWidget(parent, f)
{
    setupUi(this); // this sets up GUI
	setWindowFlags(f & (~Qt::WindowMaximizeButtonHint));
}

/******************************************************************************/

void Info::setBiota(Biota b)
{
	biota = b;
}

/******************************************************************************/

#define PI 3.1415926535897932384626433832795f
#define TO_DEGREE (180.0f/PI)

void Info::showEvent ( QShowEvent * event )
{
	Q_UNUSED(event)

	labelReproducao->setText(QVariant(biota.genes.limiar_reproducao).toString() + tr(" wens"));
	labelDistribuicao->setText(QVariant((int)(biota.genes.distribuicao_energia * 100.0f)).toString() + tr(" %"));
	labelAnguloColisao->setText(QVariant((int)(biota.genes.angulo_colisao*TO_DEGREE)).toString() + " " + tr("degrees"));
	labelAnguloReproducao->setText(QVariant((int)(biota.genes.angulo_reproducao*TO_DEGREE)).toString() + " " + tr("degrees"));
}

/******************************************************************************/
