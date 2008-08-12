#ifndef _INFO_H
#define _INFO_H
 
#include "ui_info.h"
#include "../core/biota.h"
 
class Info : public QWidget, private Ui::Info
{
    Q_OBJECT
 
public:
    Info(QWidget *parent = 0, Qt::WFlags f = 0);

	void setBiota(Biota b);

protected:

	void showEvent( QShowEvent * event );

private:

	Biota biota;
};

#endif
