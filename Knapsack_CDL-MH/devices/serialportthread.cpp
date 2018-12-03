#include "serialportthread.h"
#include "serialportthread.h"
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QDebug>

QT_USE_NAMESPACE
serialportThread::serialportThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{

}
serialportThread::~serialportThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}
void serialportThread::init(const QString &portName, const int baud, const int waitTimeout,
                            const int waitForReadyReadTime)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = waitTimeout;
    this->baud = baud;
    this->waitForReadyReadTime = waitForReadyReadTime;
}
void serialportThread::transaction(const QByteArray &request)
{
    QMutexLocker locker(&mutex);
    this->request = request;
    if (!isRunning())
    {
        start();
    }
    else
    {
        cond.wakeOne();
    }
}

void serialportThread::run()
{
    bool currentPortNameChanged = false;
    mutex.lock();
    QString currentPortName;
    if (currentPortName != portName)
    {
        currentPortName = portName;
        currentPortNameChanged = true;
    }
    int currentWaitTimeout = waitTimeout;
    QByteArray currentRequest = request;
    mutex.unlock();
    QSerialPort serial;
    while (!quit)
    {
        if (currentPortNameChanged)
        {
            serial.close();
            serial.setPortName(currentPortName);
            if(!serial.open(QIODevice::ReadWrite))
            {
                emit PortNotOpen(QString::fromLocal8Bit("���������쳣��"));
                return;
            }
            switch (baud)
            {
            case 4800:
                serial.setBaudRate(QSerialPort::Baud4800);
                break;
            case 9600:
                serial.setBaudRate(QSerialPort::Baud9600);
                break;
            case 19200:
                serial.setBaudRate(QSerialPort::Baud19200);
                break;
            case 115200:
                serial.setBaudRate(QSerialPort::Baud115200);    //������
                break;
            default:
                break;
            }
            serial.setDataBits(QSerialPort::Data8);				//����λ
            serial.setParity(QSerialPort::NoParity);			//У��λ
            serial.setStopBits(QSerialPort::OneStop);			//ֹͣλ
            serial.setFlowControl(QSerialPort::NoFlowControl);	//������
        }
        QByteArray requestData = currentRequest;
        serial.write(requestData);

        if (serial.waitForBytesWritten(waitTimeout))                        //���ֽ�д��ʱ�Ϳ�����ʱ����ֹ������
        {
            if (serial.waitForReadyRead(currentWaitTimeout))                //���źſɶ�ʱ�Ϳ�����ʱ����ֹ������
            {
                QByteArray responseData = serial.readAll();                  //�ȶ�ȡһ��
                while (serial.waitForReadyRead(waitForReadyReadTime))        // �����waitForReadyReadTime�ڻ��пɶ����ݾͼ�����ȡ
                {
                    responseData += serial.readAll();                        //����ȡ�ź�ƴ��Ϊ�����Ľ����ź�
                }

                emit this->response(responseData);                           //��������ͳһ��QByteArray�෢�ͣ���������������������
            }
            else
            {
                emit this->timeout(QString::fromLocal8Bit("�������ݶ�ȡ��ʱ"));
               return;
            }

        }
        else
        {
            emit this->timeout(QString::fromLocal8Bit("�������ݶ�ȡ��ʱ"));
         return;
        }
        mutex.lock();
        cond.wait(&mutex);
        if(currentPortName != portName)
        {
            currentPortName = portName;
            currentPortNameChanged = true;
        }
        else
            currentPortNameChanged = false;

        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
}
