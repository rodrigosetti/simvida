#include <QtGui> 
#include "stat.h"

/******************************************************************************/
 
Stat::Stat(QWidget *parent, Qt::WFlags f) : QWidget(parent, f)
{
    setupUi(this); // this sets up GUI
	setWindowFlags(f & (~Qt::WindowMaximizeButtonHint));

	imgBiotasGraos->initImagem();
	imgEspecies->initImagem();
}

/******************************************************************************/

