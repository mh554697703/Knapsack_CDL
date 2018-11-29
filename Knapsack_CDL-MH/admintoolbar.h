#ifndef ADMINTOOLBAR_H
#define ADMINTOOLBAR_H
#include "QToolBar"
#include "QString"
#include "QAction"

class AdminToolBar : public QToolBar
{
public:
    AdminToolBar();
    QAction *setAction;
};

#endif // ADMINTOOLBAR_H
