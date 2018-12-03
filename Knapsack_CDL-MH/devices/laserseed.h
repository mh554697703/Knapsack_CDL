#ifndef LASERSEED_H
#define LASERSEED_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QByteArray>
#include "serialportthread.h"
#include "laserpulse.h"

class laserSeed : public QObject
{
    Q_OBJECT
public:
    explicit laserSeed(QObject *parent = nullptr);
    void beginSeedLaser(const double &SeedPower,const double &PulsePower);
    void setSeedPower(const int &s);
    void closeSeedLaser();
    void checkLaser();
private slots:
    void receive_response(const QByteArray &s);
    void portError(const QString &s);

signals:
//    void powerReady();
    void laserSeedError(const QString &s);
    void seedOpenReady(const double &SeedPower);
    void laserColseRight();
    void laserSeedOk();
private:
    QByteArray senddata;
    QString laserPort,errorCode;
    bool powerSet, fire, close, openPulse;
    serialportThread laserseedThread;
    double seedPower,pulsePower;

    int baud,waittimeout,waitForReadyReadTime;
};

#endif // LASERSEED_H
