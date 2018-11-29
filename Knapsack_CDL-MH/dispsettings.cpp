#include "dispsettings.h"

DispSettings::DispSettings(SOFTWARESETTINGS settings)
{
    dispSettings = settings;
    qDebug() << "laserPulseEnergy = " << dispSettings.laserPulseEnergy;
    qDebug() << "laserLocalPower = " << dispSettings.laserLocalPower;
    qDebug() << "laserRPF = " << dispSettings.laserRPF;
    qDebug() << "laserPulseWidth = " << dispSettings.laserPulseWidth;
    qDebug() << "laserWaveLength = " << dispSettings.laserWaveLength;
    qDebug() << "laserAOMFreq = " << dispSettings.laserAOMFreq;

    qDebug() << "detectMode = " << dispSettings.detectMode;
    qDebug() << "elevationAngle = " << dispSettings.elevationAngle;
    qDebug() << "azAngleStart = " << dispSettings.azAngleStart;
    qDebug() << "azAngleStep = " << dispSettings.azAngleStep;
    qDebug() << "angleNum = " << dispSettings.angleNum;
    qDebug() << "circleNum = " << dispSettings.circleNum;
    qDebug() << "isAngleChecked = " << dispSettings.isAngleChecked;
    qDebug() << "isCircleChecked = " << dispSettings.isCircleChecked;
    qDebug() << "motorSpeed = " << dispSettings.motorSpeed;
    qDebug() << "intervalTime = " << dispSettings.intervalTime;
    qDebug() << "groupTime = " << dispSettings.groupTime;

    qDebug() << "sampleFreq = " << dispSettings.sampleFreq;
    qDebug() << "triggerLevel = " << dispSettings.triggerLevel;
    qDebug() << "nPointsPreTrigger = " << dispSettings.nPointsPreTrigger;

    qDebug() << "nPulsesAcc = " << dispSettings.nPulsesAcc;
    qDebug() << "nRangeBin = " << dispSettings.nRangeBin;
    qDebug() << "nPointsPerBin = " << dispSettings.nPointsPerBin;
    qDebug() << "nPointsMirrorWidth = " << dispSettings.nPointsMirrorWidth;
    qDebug() << "overlapRatio = " << dispSettings.overlapRatio;

    qDebug() << "nPointsObjFreq = " << dispSettings.nPointsObjFreq;
    qDebug() << "nDirsVectorCal = " << dispSettings.nDirsVectorCal;

    qDebug() << "dataFilePath = " << dispSettings.dataFilePath;
    qDebug() << "autoCreateDateDir = " << dispSettings.autoCreateDateDir;
    qDebug() << "nDirsPerFile = " << dispSettings.nDirsPerFile;
}
