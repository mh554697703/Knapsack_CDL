#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H
#include<QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>


class serialportThread : public QThread
{
    Q_OBJECT
public:

    serialportThread(QObject *parent = nullptr);
        ~serialportThread();
        void init(const QString &portName,const int baud, const int waitTimeout,
                  const int waitForReadyReadTime);
        void transaction(const QByteArray &request);
        void run() Q_DECL_OVERRIDE;
    signals:
        void response(const QByteArray &s);
        void PortNotOpen(const QString &s);
        void timeout(const QString &s);

    private:
        QString portName;
        QByteArray request;
        int baud,waitTimeout,waitForReadyReadTime;

        QMutex mutex;
        QWaitCondition cond;
        bool quit;
};
#endif // SERIALPORTTHREAD_H
