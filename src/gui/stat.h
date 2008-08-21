#ifndef _STAT_H
#define _STAT_H
 
#include "ui_stat.h"
 
class Stat : public QWidget, public Ui::Stat
{
    Q_OBJECT

public:
    Stat(QWidget *parent = 0, Qt::WFlags f = 0);
};

#endif

