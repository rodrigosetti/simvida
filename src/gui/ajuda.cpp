#include <QtGui> 
#include "ajuda.h"

// if we include <QtGui> there is no need to include every class used: <QString>, <QFileDialog>,...
 
Ajuda::Ajuda(QWidget *parent, Qt::WFlags f) : QWidget(parent, f)
{
    setupUi(this); // this sets up GUI
	setWindowFlags(f & (~Qt::WindowMaximizeButtonHint));

	/* Conecta botao */
	QObject::connect(buttonOk, SIGNAL(clicked()), this, SLOT(close()));
}

