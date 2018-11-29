#ifndef LASERPULSE_H
#define LASERPULSE_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QByteArray>
#include "serialportthread.h"
#include "laserseed.h"

class laserPulse : public QObject
{
    Q_OBJECT
public:
    explicit laserPulse(QObject *parent = nullptr);

public slots:
    void setPulsePower(const int &s);
    void checkLaser();
    void closePulseLaser();
    void beginPulseLaser(const double &s);

private slots:
    void receive_response(const QByteArray &s);
    void portError();
    void timeout();


signals:
//    void powerReady();
    void laserPulseError(QString &s);
    void pulseCloseReady();
    void laserPulseOk();
private:
    QByteArray senddata;
    QString laserPort,errorCode;
    bool powerSet,fire,close;
    serialportThread laserpulseThread;
    double pulsePower;
    int baud,waittimeout,waitForReadyReadTime;
};
#endif // laserPulse_H
