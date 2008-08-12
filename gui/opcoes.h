#ifndef _OPCOES_H
#define _OPCOES_H
 
#include <QScrollBar>
#include "ui_opcoes.h"
#include "../core/mundo.h"
 
class Opcoes : public QWidget, private Ui::Opcoes
{
    Q_OBJECT
 
public:
    Opcoes(QWidget *parent = 0, Qt::WFlags f = 0);

	void setMundo(Mundo *m);
	void setBarras(QScrollBar *x, QScrollBar *y);

public slots:

	void aceitou();

protected:

	void showEvent( QShowEvent * event );

private:

	Mundo *mundo;
	QScrollBar *barX, *barY;
};

#endif

