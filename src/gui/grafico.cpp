#include <QPainter>
#include <QPaintEvent>
#include <QBrush>
#include "grafico.h"

/******************************************************************************/

Grafico::Grafico(QWidget *parent, Qt::WFlags f) : QWidget(parent, f)
{
	imagem = NULL;
}

/******************************************************************************/

void Grafico::initImagem()
{
	if (imagem == NULL)
	{
		imagem = new QImage(width(), height(), QImage::Format_RGB32);

		QPainter painterI(imagem);
		painterI.setBrush(QBrush(Qt::white));
		painterI.setPen(QPen(Qt::NoPen));
		painterI.drawRect(0,0,imagem->width(), imagem->height());
	}
}

/******************************************************************************/

/* Desenhar */
void Grafico::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)

	/* pinta imagem no widget */
	QPainter painterW(this);
	painterW.drawImage(0,0,*imagem);
}

/******************************************************************************/
