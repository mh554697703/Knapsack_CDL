#include "global_defines.h"
#include "motor.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

motor::motor(QObject *parent) : QObject(parent)
{
    moveTime = new QTime();
    checkMoveTimer = new QTimer(this);
    connect(&motorThread,SIGNAL(response(QByteArray)),this,SLOT(receive_response(QByteArray)));
    connect(&motorThread,SIGNAL(PortNotOpen(QString)),this,SLOT(PortError(QString)));
    connect(&motorThread,SIGNAL(timeout(QString)),this,SLOT(PortError(QString)));
    connect(checkMoveTimer,SIGNAL(timeout()),this,SLOT(checkMove()));

    moveNorth = false;
    baud = 19200;
    waittimeout = 300;
    waitForReadyReadTime = 80;
    motorThread.init(MotorComPort,baud,waittimeout,waitForReadyReadTime);
}

void motor::prepare(const double &a)
{
    //portname = MotorComPort;
    HeadAngle = a;
    moveNorth = true;
    Order_str = "VR;";

    motorThread.transaction(Order_str.toLatin1());   //获取版本后依次上电--设置加减速度--设置速度--发送上电完成信号
}

void motor::set_Motor_azAngleStep(const double &s)
{
    mysetting.azAngleStep = s;
}

void motor::moveAbsolute(const double &a)
{
    QString anglePA = QString::number(a*524288/360,'f',2);
    qDebug()<<"a="<<anglePA;
    Order_str = "PA="+anglePA+";";

    motorThread.transaction(Order_str.toLatin1());
}

void motor::moveRelative(const double &a)
{
    qDebug()<<"moveRelative";
    moveTime->start();
    QString anglePR=QString::number(a*524288/360,'f',2);
    Order_str = "PR="+anglePR+";";

    motorThread.transaction(Order_str.toLatin1());
}

void motor::position()
{
    Order_str = "PX;";

    motorThread.transaction(Order_str.toLatin1());
}

void motor::checkMove()
{
    Order_str = "MS;";

    motorThread.transaction(Order_str.toLatin1());
}

void motor::checkMotorAngle(const double &s)
{qDebug()<<"motor back="<<s;
    if(moveNorth)       // 采集之初，驱动电机指向正北时
    {
        if(HeadAngle-s>=-0.5&&HeadAngle-s<=0.5)
        {
            moveNorth = false;
            motorPX0 = s;
            if(motorPX0<mysetting.azAngleStep)
                motorPX0 = motorPX0+360;
            emit this->positionArrived(motorPX0);
        }
        else
        {
            this->moveRelative(HeadAngle-s);
        }
    }
    else                // 每次采集结束，转到下一方向时
    {
        if((motorPX0-s-mysetting.azAngleStep)<=0.01&&(motorPX0-s-mysetting.azAngleStep)>=-0.01)   //判断是否到达指定位置,误差暂设0.5°
        {
            motorPX0 = s;
            if(motorPX0<mysetting.azAngleStep)
                motorPX0 = motorPX0+360;
            emit this->positionArrived(motorPX0);
            int motorMoveTime = moveTime->elapsed();
            qDebug()<<"motorMoveTime = "<<motorMoveTime;
        }
        else
        {
            this->moveRelative(motorPX0-mysetting.azAngleStep-s);
        }

    }
}

void motor::motorQuit()
{
    Order_str = "MO=0;";
    motorThread.transaction(Order_str.toLatin1());
}

void motor::receive_response(const QByteArray &responseInfo)
{
    qDebug()<<"responseInfo = "<<responseInfo;
    QString responseStr = QString(responseInfo);
    if(responseStr.left(2) == "MO")                       //电机是否关闭
    {
        if(responseStr.left(4) == "MO=1")
        {
            Order_str = "AC=1e7;";

            motorThread.transaction(Order_str.toLatin1());
        }
        else
        {
            emit this->motorClosed();
            qDebug()<<"motor close";
        }
    }
    if(responseStr.left(2) == "AC")
    {
        Order_str = "DC=1e7;";

        motorThread.transaction(Order_str.toLatin1());
    }
    if(responseStr.left(2) == "BG")
    {
        qDebug()<<"BG";
        checkMoveTimer->start(motorCheckPeriod);        //转动过程中，用于电机MS判断的定时器，查询间隔（150ms）
    }
    if(responseStr.left(2) == "DC")
    {
        Order_str = "SP=524288;";

        motorThread.transaction(Order_str.toLatin1());
    }
    if(responseStr.left(2) == "MS")
    {
        if(responseStr.left(4)=="MS;0")
        {
                checkMoveTimer->stop();
            Order_str = "PX;";

            motorThread.transaction(Order_str.toLatin1());
        }
        else
        {
            if(responseStr.left(4)=="MS;3")
            {
                if(checkMoveTimer->isActive())
                {
                    checkMoveTimer->stop();
                }
                emit this->motorError("Reserved?");
            }
        }
    }
    if(responseStr.left(2) == "PA")
    {
        Order_str = "BG;";

        motorThread.transaction(Order_str.toLatin1());
    }
    if(responseStr.left(2) == "PR")
    {
        qDebug()<<"PR1";
        Order_str = "BG;";

        motorThread.transaction(Order_str.toLatin1());
        qDebug()<<"PR2";
    }
    if(responseStr.left(2) == "PX")
    {
        QString ret = responseStr.split(";").at(1);	//PX值
        int retVal = ret.toInt();
        int temp = retVal & 0x7ffff;
        double angle = (double) temp/524288*360.0;

        while(angle<0)
            angle = angle + 360;

        while(angle>360)
            angle = angle - 360;
        this->checkMotorAngle(angle);
    }
    if(responseStr.left(2) == "SP")
    {
        emit this->motorPrepareOk();          //此信号连接的槽函数仅仅输出一句话，没有实际作用
        this->position();                     //电机prepare工作结束后，直接开始获取电机位置以及后续操作
    }
    if(responseStr.left(10) == "VR;Whistle")
    {
        Order_str = ";";

        motorThread.transaction(Order_str.toLatin1());
    }
    if(responseStr.left(1) == ";")
    {
        qDebug() << "return ;;;;;;;;;;;;;;;;;;;;;;;;;;";
        Order_str = "MO=1;";

        motorThread.transaction(Order_str.toLatin1());
    }
}

void motor::PortError(const QString &s)
{
    QString errorCode = QString::fromLocal8Bit("电机");
    errorCode.append(s);
    emit motorError(errorCode);
}
