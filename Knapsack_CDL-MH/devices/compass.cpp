#include "global_defines.h"
#include "compass.h"
#include <QDebug>
#include <QByteArray>

compass::compass(QObject *parent)
{
    connect(&compassThread, &serialportThread::response, this, &compass::showResponse);
    connect(&compassThread, &serialportThread::PortNotOpen, this, &compass::portError);
    connect(&compassThread, &serialportThread::timeout, this, &compass::portError);

    baud = 9600;
    waittimeout = 3000;
    waitForReadyReadTime = 30;

    compassThread.init(CompassComPort,baud,waittimeout,waitForReadyReadTime);
}

void compass::read()
{
    //要发送的命令
    QByteArray str = "6804000408";
    senddata=QByteArray::fromHex(str);

    compassThread.transaction(senddata);
}

//SXXX.YYS 为符号位（0 正，1 负）XXX 为三位整数值，YY 为两位位小数值。如 10 26 87 表示-26.87o
double compass::toangle(const QString &str_Angle)
{
    double answer;
    QString sign = str_Angle.mid(0,1);                    //截取符号位，整数位和小数位
    QString integers = str_Angle.mid(1,3);
    QString decimals = str_Angle.mid(4,2);
    answer = integers.toDouble()+0.01*decimals.toDouble(); //数据整合为实际角度
    if(sign.toInt() == 1)
        answer = -1*answer;               //符号位为1时取负
    return answer;
}

void compass::showResponse(const QByteArray &s)
{
    double headAngle;
    QString temp = s.toHex();           //接收信号转16进制

    QString str_pitch, str_roll, str_head;
    //截取接收信号 pitch roll head
    str_pitch = temp.mid(8,6);
    str_roll = temp.mid(14,6);
    str_head = temp.mid(20,6);

    headAngle = toangle(str_head);

    emit this->compassAngle(headAngle);
}

void compass::portError(const QString &s)
{
    QString errorCode = QString::fromLocal8Bit("罗盘");
    errorCode.append(s);
    emit compassError(errorCode);
}
