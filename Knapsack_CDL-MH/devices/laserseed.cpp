#include "laserseed.h"
#include "global_defines.h"
#include <qdebug.h>

laserSeed::laserSeed(QObject *parent) : QObject(parent)
{
    connect(&laserseedThread,SIGNAL(response(QByteArray)),this,SLOT(receive_response(QByteArray)));
    connect(&laserseedThread,&serialportThread::PortNotOpen, this,&laserSeed::portError);
    connect(&laserseedThread,SIGNAL(timeout(QString)),this,SLOT(portError(QString)));

    //    laserPort = "COM1";
    powerSet = true;
    fire = false;
    close = false;

    baud = 9600;
    waittimeout = 3000;
    waitForReadyReadTime = 100;        //ԭ����30���룬���ǻ���ַ������ݶ������������ӳ���100

    laserseedThread.init(SeedLaserComPort,baud,waittimeout,waitForReadyReadTime);
}

void laserSeed::beginSeedLaser(const double &SeedPower,const double &PulsePower)
{
    qDebug() << "begin Seed laser";
    seedPower = SeedPower;
    pulsePower = PulsePower;
    senddata = QByteArray::fromHex("AA 55 C1 01 01 01 00");


    laserseedThread.transaction(senddata);
    fire = true;
    openPulse = true;
}

void laserSeed::setSeedPower(const int &s)
{
    QString key = QString("%1").arg(s,4,16,QLatin1Char('0')).toUpper();
    bool ok;
    int aa = key.left(2).toInt(&ok,16)+key.right(2).toInt(&ok,16);
    QString key2 = QString("%1").arg(aa,4,16,QLatin1Char('0')).toUpper();
    QString power = "AA 55 C3 02 "+key.right(2)+" "+key.left(2)+" "+key2.right(2)+" "+key2.left(2);

    senddata = QByteArray::fromHex(power.toLatin1());

    laserseedThread.transaction(senddata);
    powerSet = false;
}

void laserSeed::closeSeedLaser()
{
    close = true;

    senddata = QByteArray::fromHex("AA 55 C1 01 00 00 00");

    laserseedThread.transaction(senddata);

}

void laserSeed::receive_response(const QByteArray &s)
{
    QString temp = s.toHex();
    if(!powerSet)
    {
        QString powerAnswer = temp.mid(8,2);
        if(powerAnswer!= "00")
        {
            errorCode = QString::fromLocal8Bit("����Դ�������������ô���");
            emit this->laserSeedError(errorCode);
        }
        else
        {
            powerSet = true;
            if(openPulse)
            {
                emit this->seedOpenReady(pulsePower);
                openPulse = false;
            }
            qDebug()<<QString::fromLocal8Bit("����Դ�������������óɹ�");
        }
    }
    else
    {
        if(fire||close)
        {
            if(fire)
            {
                if(temp == "55aac101000000")
                {
                    //                    setSeedPower(1000); //������
                    setSeedPower((int)(seedPower*1000));
                    qDebug()<<QString::fromLocal8Bit("����Դ������������");
                }
                else
                {
                    errorCode =QString::fromLocal8Bit("����Դ���������쳣");
                    emit this->laserSeedError(errorCode);
                }
                fire = false;
            }
            else
            {
                if(temp == "55aac101000000")
                {
                    emit this->laserColseRight();
                    qDebug()<<"seed close right";
                }
                else
                {
                    errorCode = QString::fromLocal8Bit("����Դ�������ر��쳣");
                    emit this->laserSeedError(errorCode);
                }
                close = false;
            }

        }
        else
        {
            QString checkAnswer = temp.mid(8,2);
            if(checkAnswer == "00")
            {
                errorCode = QString::fromLocal8Bit("����Դ���������쳣");
                emit this->laserSeedError(errorCode);
            }
            else
            {
                QString checkAnswer = temp.mid(10,2);
                if(checkAnswer!= "00")
                {
                    bool ok;
                    int s = checkAnswer.right(1).toInt(&ok,16);
                    QString key = QString("%1").arg(s,4,2,QLatin1Char('0'));
                    if(key.left(1) == "1")
                    {qDebug()<<QString::fromLocal8Bit("�����¶��쳣");
                        errorCode.append(QString::fromLocal8Bit("�����¶��쳣;"));}
                    if(key.right(1) == "1")
                    {qDebug()<<QString::fromLocal8Bit("ģ���¶��쳣");
                        errorCode.append(QString::fromLocal8Bit("ģ���¶��쳣;"));}
                    if(key.mid(1,1) == "1")
                    {qDebug()<<QString::fromLocal8Bit("���빦���쳣");
                        errorCode.append(QString::fromLocal8Bit("���빦���쳣;"));}
                    if(key.mid(2,1) == "1")
                    {qDebug()<<QString::fromLocal8Bit("���Ӽ������¶��쳣");
                        errorCode.append(QString::fromLocal8Bit("���Ӽ������¶��쳣;"));}
                    emit this->laserSeedError(errorCode);
                }
                else
                {
                    qDebug()<<QString::fromLocal8Bit("����Դ��������������");
                    emit this->laserSeedOk();
                }
            }
        }
    }
}

void laserSeed::checkLaser()
{
    qDebug()<<"check LaserSeed";
    //    StringToHex("AA 55 D3 00 00 00",senddata);
    senddata = QByteArray::fromHex("AA 55 D3 00 00 00");

    laserseedThread.transaction(senddata);
}

void laserSeed::portError(const QString &s)
{
    QString errorCode = QString::fromLocal8Bit("����Դ������");
    errorCode.append(s);
    emit laserSeedError(errorCode);
}
