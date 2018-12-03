#ifndef COMPASS_H
#define COMPASS_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

#include <QByteArray>
#include "serialportthread.h"

class compass : public QObject
{
    Q_OBJECT

public:
    explicit compass(QObject *parent = nullptr );
    void read();

signals:
    void compassAngle(const double &s);
    void compassError(const QString &s);
public slots:
    void portError(const QString &s);
private:
    QByteArray senddata;                                   //需要发送的字节数组
    double toangle(const QString &c);                     //将接收的数据转为角度信息
    void showResponse(const QByteArray &s);

    serialportThread compassThread;
    int baud,waittimeout,waitForReadyReadTime;

};
#endif // COMPASS_H
