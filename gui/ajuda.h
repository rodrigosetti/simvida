#ifndef _AJUDA_H
#define _AJUDA_H
 
#include "ui_ajuda.h"
 
class Ajuda : public QWidget, private Ui::Ajuda
{
    Q_OBJECT
 
public:
    Ajuda(QWidget *parent = 0, Qt::WFlags f = 0);

};

#endif
