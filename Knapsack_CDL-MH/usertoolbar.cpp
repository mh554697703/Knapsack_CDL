#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    startAction = this->addAction(QString::fromLocal8Bit("开始"));
//    addSeparator();
//    stopAction = this->addAction(QString::fromLocal8Bit("停止"));
    addSeparator();
    setAction = this->addAction(QString::fromLocal8Bit("设置"));
    addSeparator();
    quitAction = this->addAction(QString::fromLocal8Bit("退出"));

    setIconSize(QSize(35, 35));
    startAction->setIcon(QIcon(":/images/Start.png"));   
    setAction->setIcon(QIcon(":/images/Gear.png"));
    quitAction->setIcon(QIcon(":/images/Standby.png"));
//    stopAction->setIcon(QIcon(":/images/Stop.png"));
    setMovable(false);
    startIcon = true;       // 当前图标，start
    connect(startAction,&QAction::triggered,this,&UserToolBar::changeStartIcon);
}

void UserToolBar::changeStartIcon()
{
    if(startIcon)
    {
        startAction->setIcon(QIcon(":/images/Stop.png"));
        startAction->setToolTip(QString::fromLocal8Bit("停止"));
        startIcon = false;
    }
    else
    {
        startAction->setIcon(QIcon(":/images/Start.png"));
        startAction->setToolTip(QString::fromLocal8Bit("开始"));
        startIcon = true;
    }
}

void UserToolBar::set_to_stopped()      //设定为停止状态，即图标为可以开始
{
    startAction->setIcon(QIcon(":/images/Start.png"));
    startIcon = true;
}
 void UserToolBar::set_to_started()
 {
     startAction->setIcon(QIcon(":/images/Stop.png"));
     startIcon =false;
 }
