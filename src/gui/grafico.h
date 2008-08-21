#ifndef _GRAFICO_H
#define _GRAFICO_H
 
#include <QWidget>
#include <QImage>
 
class Grafico : public QWidget
{
    Q_OBJECT

public:
    Grafico(QWidget *parent = 0, Qt::WFlags f = 0);

	QImage *imagem;
	void initImagem();

protected:

	/* Desenhar */
	void paintEvent(QPaintEvent *event);
};

#endif

