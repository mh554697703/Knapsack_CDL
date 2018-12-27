#ifndef USERTOOLBAR_H
#define USERTOOLBAR_H
#include "QToolBar"
#include "QAction"
#include "QString"

class UserToolBar : public QToolBar
{
public:
    UserToolBar();
    QAction *startAction,*stopAction,*setAction,*quitAction;
public slots:
    void changeStartIcon();
    void set_to_stopped();
    void set_to_started();
private:
    bool startIcon;     // ��ʼ�ɼ���ť�ĵ�ǰ״̬��tureΪ�ɼ���falseΪֹͣ
};

#endif // USERTOOLBAR_H
