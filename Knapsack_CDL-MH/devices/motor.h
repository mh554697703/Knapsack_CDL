#ifndef MOTOR_H
#define MOTOR_H

#include<QTimer>
#include <QObject>
#include "serialportthread.h"
#include <global_defines.h>
#include <QTime>

class motor : public QObject
{
    Q_OBJECT
public:
    explicit motor(QObject *parent = nullptr);
    void moveAbsolute(const double &a);
    void moveRelative(const double &a);
    void position();
    void checkMotorAngle(const double &s);
    void motorQuit();
    void set_Motor_azAngleStep(const double &s);
signals:
    void motorPrepareOk();
    void positionArrived(const double &s);
    void motorAngle(const double &s);
    void motorClosed();
    void beginMove();
    void moveReady();
    void motorError(const QString s);
public slots:
    void prepare(const double &a);
private slots:
    void PortError(const QString &s);
    void receive_response(const QByteArray &responseInfo);
    void checkMove();
private:
    double HeadAngle;       //获取罗盘角度
    bool moveNorth;         //判断是否需要指北
    double motorPX0;        //获取电机角度
    SOFTWARESETTINGS mysetting;

    QString portname, Order_str, AC, DC, PR, PA, SP;
    //    串口名   发送命令 加减速度 相对/绝对移动 转速
    serialportThread motorThread;
    QTimer *checkMoveTimer = NULL;     //用于检查电机是否在转动

    int baud, waittimeout, waitForReadyReadTime;
    QTime *moveTime;

};

#endif // MOTOR_H



