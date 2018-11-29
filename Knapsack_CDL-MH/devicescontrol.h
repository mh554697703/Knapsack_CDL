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
    void Create_DataFolder();       // 数据存储文件夹的创建
    void beginWork();         //采集过程控制主体
    void mySleep(unsigned int msec);           //延时

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

    void Generate_freqAxis();               // 生成频率坐标轴
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
    double currentMotorAngle;       //电机的当前位置，是否就是motorPX0？
    double headAngle;               //记录罗盘位置
    bool Stop_now;                  // 命令：GUI用来控制ControlTimer停止采集，待商榷
    uint capture_counter;           // 探测方向计数器
    QDateTime Start_Time;           // 开始时间，用于定时探测模式
    Control_State State;

    SOFTWARESETTINGS mysetting;

    unsigned int nRB_ovlp;                   // 修正了overlap之后的距离门数，不包括噪声和镜面
    double freqAxis [nFFT_half];    // 频率轴
    double *losVelocity = nullptr;            // 径向风速值
    double *aomSpec = nullptr;
    double *specArray = nullptr;

    VectorXd azimuthAngle;
    MatrixXd losVelocityMat;
    double *hVelocity = nullptr;
    double *hAngle = nullptr;
    double *vVelocity = nullptr;
    double Height_values[Max_nLayers];

    double *vectorVelocity = nullptr;

    QDateTime CaptureTime;          // 当前方向的采集时间
    QString SpecFileName;
    QString VeloFileName;
    QTime *CM_Time, *workTime;
    QTime *CS_Time;

    QThread *Motorthread;           //motor工作的线程
};

#endif // DEVICESCONTROL_H
