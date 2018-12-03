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
                emit PortNotOpen(QString::fromLocal8Bit("串口连接异常！"));
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
                serial.setBaudRate(QSerialPort::Baud115200);    //波特率
                break;
            default:
                break;
            }
            serial.setDataBits(QSerialPort::Data8);				//数据位
            serial.setParity(QSerialPort::NoParity);			//校验位
            serial.setStopBits(QSerialPort::OneStop);			//停止位
            serial.setFlowControl(QSerialPort::NoFlowControl);	//流控制
        }
        QByteArray requestData = currentRequest;
        serial.write(requestData);

        if (serial.waitForBytesWritten(waitTimeout))                        //有字节写入时就开启，时间终止后跳出
        {
            if (serial.waitForReadyRead(currentWaitTimeout))                //有信号可读时就开启，时间终止后跳出
            {
                QByteArray responseData = serial.readAll();                  //先读取一次
                while (serial.waitForReadyRead(waitForReadyReadTime))        // 如果在waitForReadyReadTime内还有可读数据就继续读取
                {
                    responseData += serial.readAll();                        //将读取信号拼接为完整的接收信号
                }

                emit this->response(responseData);                           //返回命令统一以QByteArray类发送，具体器件各自再做处理
            }
            else
            {
                emit this->timeout(QString::fromLocal8Bit("串口数据读取超时"));
               return;
            }

        }
        else
        {
            emit this->timeout(QString::fromLocal8Bit("串口数据读取超时"));
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
