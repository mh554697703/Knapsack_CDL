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
private:
    bool startIcon;     // 开始采集按钮的当前状态，ture为采集，false为停止
};

#endif // USERTOOLBAR_H
