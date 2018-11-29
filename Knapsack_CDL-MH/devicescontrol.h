#ifndef DEVICESCONTROL_H
#define DEVICESCONTROL_H

#include <QObject>
#include "devices/adq214.h"
#include "devices/compass.h"
#include "devices/motor.h"
#include "devices/laserpulse.h"
#include "devices/laserseed.h"
#include <global_defines.h>
#include <ADQAPI.h>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QFileInfo>
#include <QDataStream>
#include <QDateTime>
#include <dispsettings.h>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include "calculate/dswf.h"
#include "calculate/fswf.h"
#include <QThread>
#include <QCoreApplication>
using namespace Eigen;

class DevicesControl : public QObject
{
    Q_OBJECT
public:
    explicit DevicesControl(QObject *parent = nullptr);
    void startAction(SOFTWARESETTINGS settings);
    void stopAction();
    void Create_DataFolder();       // ���ݴ洢�ļ��еĴ���
    void beginWork();         //�ɼ����̿�������
    void mySleep(unsigned int msec);           //��ʱ

signals:
    void hVelocityReady(double *hVelocity,uint n);
    void hAngleReady(double *hAngle,uint n);
    void vVelocityReady(double *vVelocity,uint n);
    void detectionFinished();
    void beginMotor(const double &s);
    void motorError(const QString s);
    void compassError(const QString &s);
    void laserseedError(const QString &s);
    void laserpulseError(const QString &s);
    void adqError();

private slots:

    void SetMotorFlag(const double &s);
    void laserReady();

    void checkMotor();    

    void readyToMove();
    void motorCloseOk();
    void laserCloseOk();
    void pulse_laser_opened_fcn(const double &s);

    void Generate_freqAxis();               // ����Ƶ��������
    void LOSVelocityCal(const unsigned int heightNum, const uint totalSpecPoints, const uint objSpecPoints, const double lambda, const double *freqAxis, const double *specData);
    void Init_Buffers();

    void CalHeightsValues();
    void SaveSpec_FileHead();
    void SaveSpec_AddData();
    void SaveVelo_AddData();
    void SaveVelo_FileHead();

private:
    compass Compass;
    motor *Motor;
    laserSeed LaserSeed;
    laserPulse LaserPulse;
    ADQ214 adq;
    FSWF fswf;

    bool keepWorking,motorInPosition,LaserReady,MotorCloseOk,LaserCloseOk;
    double currentMotorAngle;       //����ĵ�ǰλ�ã��Ƿ����motorPX0��
    double headAngle;               //��¼����λ��
    bool Stop_now;                  // ���GUI��������ControlTimerֹͣ�ɼ�������ȶ
    uint capture_counter;           // ̽�ⷽ�������
    QDateTime Start_Time;           // ��ʼʱ�䣬���ڶ�ʱ̽��ģʽ
    Control_State State;

    SOFTWARESETTINGS mysetting;

    unsigned int nRB_ovlp;                   // ������overlap֮��ľ��������������������;���
    double freqAxis [nFFT_half];    // Ƶ����
    double *losVelocity = nullptr;            // �������ֵ
    double *aomSpec = nullptr;
    double *specArray = nullptr;

    VectorXd azimuthAngle;
    MatrixXd losVelocityMat;
    double *hVelocity = nullptr;
    double *hAngle = nullptr;
    double *vVelocity = nullptr;
    double Height_values[Max_nLayers];

    double *vectorVelocity = nullptr;

    QDateTime CaptureTime;          // ��ǰ����Ĳɼ�ʱ��
    QString SpecFileName;
    QString VeloFileName;
    QTime *CM_Time, *workTime;
    QTime *CS_Time;

    QThread *Motorthread;           //motor�������߳�
};

#endif // DEVICESCONTROL_H
