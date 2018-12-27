#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    startAction = this->addAction(QString::fromLocal8Bit("��ʼ"));
//    addSeparator();
//    stopAction = this->addAction(QString::fromLocal8Bit("ֹͣ"));
    addSeparator();
    setAction = this->addAction(QString::fromLocal8Bit("����"));
    addSeparator();
    quitAction = this->addAction(QString::fromLocal8Bit("�˳�"));

    setIconSize(QSize(35, 35));
    startAction->setIcon(QIcon(":/images/Start.png"));   
    setAction->setIcon(QIcon(":/images/Gear.png"));
    quitAction->setIcon(QIcon(":/images/Standby.png"));
//    stopAction->setIcon(QIcon(":/images/Stop.png"));
    setMovable(false);
    startIcon = true;       // ��ǰͼ�꣬start
    connect(startAction,&QAction::triggered,this,&UserToolBar::changeStartIcon);
}

void UserToolBar::changeStartIcon()
{
    if(startIcon)
    {
        startAction->setIcon(QIcon(":/images/Stop.png"));
        startAction->setToolTip(QString::fromLocal8Bit("ֹͣ"));
        startIcon = false;
    }
    else
    {
        startAction->setIcon(QIcon(":/images/Start.png"));
        startAction->setToolTip(QString::fromLocal8Bit("��ʼ"));
        startIcon = true;
    }
}

void UserToolBar::set_to_stopped()      //�趨Ϊֹͣ״̬����ͼ��Ϊ���Կ�ʼ
{
    startAction->setIcon(QIcon(":/images/Start.png"));
    startIcon = true;
}
 void UserToolBar::set_to_started()
 {
     startAction->setIcon(QIcon(":/images/Stop.png"));
     startIcon =false;
 }
