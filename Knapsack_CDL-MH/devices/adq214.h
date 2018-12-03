#ifndef ADQ214_H
#define ADQ214_H

#include <QObject>
#include "ADQAPI.h"
#include <QDebug>
#include <memory.h>
#include <stdio.h>
#include <QFile>
#include <QTimer>
#include "global_defines.h"

class ADQ214 : public QObject
{
    Q_OBJECT
public:
    explicit ADQ214(QObject *parent = nullptr);
    void connectADQDevice();
    bool Start_Capture();
    bool Config_ADQ214();           // ���òɼ���
    bool CaptureData2Buffer();      // �ɼ����ݵ�����
    void WriteSpecData2disk();      // д��ɼ����ݵ��ļ�
    void ConvertData2Spec();        // ת���ɼ����ݵ�������
    void Transfer_Settings(const SOFTWARESETTINGS &settings);     //���ݽ���ȫ������
    void Init_Buffers();
    PSD_DATA *get_PSD_Union();
    double *get_PSD_double();

private slots:
    void capturetimeout();
signals:
    void collectFinish();
    void adqError(const QString &s);
private:
    bool isADQ214Connected;
    void *adq_cu;
    setupADQ setupadq;
    SOFTWARESETTINGS mainSettings;
    bool success;
    PSD_DATA *psd_res;  // �洢 quint64 ��ʽ�Ĺ�����
    double *psd_array;  // �洢 double  ��ʽ�Ĺ�����

    int nPSD;
    int num_of_devices;
    int num_of_failed;
    int num_of_ADQ214;
    QTimer *captureTimer;
};

#endif // ADQ214_H
