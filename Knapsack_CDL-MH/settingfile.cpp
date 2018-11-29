#include "settingfile.h"

SettingFile::SettingFile()
{
    defaultIniFilePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    defaultIniFilePath = defaultIniFilePath + "/Default.ini";
    testSettings();
//    qDebug() << defaultIniFilePath;
//    readSettings();
}

void SettingFile::writeSettings(const SOFTWARESETTINGS &setting)      //д���ļ�
{
    fsetting = setting;
//    qDebug() << userIniFilePath;
    QSettings newSettings(userIniFilePath, QSettings::IniFormat);
    newSettings.beginGroup("Laser_Parameters");
    newSettings.setValue("laserPulseEnergy",fsetting.laserPulseEnergy);     //��������
    newSettings.setValue("laserLocalPower",fsetting.laserLocalPower);       //���⹦��
    newSettings.setValue("laserRPF",fsetting.laserRPF);                     //������Ƶ
    newSettings.setValue("laserPulseWidth",fsetting.laserPulseWidth);       //��������
    newSettings.setValue("laserWaveLength",fsetting.laserWaveLength);       //���Ⲩ��
    newSettings.setValue("laserAOMFreq",fsetting.laserAOMFreq);             //AOM��Ƶ��
    newSettings.endGroup();

    newSettings.beginGroup("Scan_Parameters");
    newSettings.setValue("detectMode",fsetting.detectMode);                 //̽��ģʽ��0����̽��1����̽��2��ʱ̽��
    newSettings.setValue("elevationAngle",fsetting.elevationAngle);         //������
    newSettings.setValue("azAngleStart",fsetting.azAngleStart);             //��ʼ��
    newSettings.setValue("azAngleStep",fsetting.azAngleStep);               //������
    newSettings.setValue("angleNum",fsetting.angleNum);                     //������
    newSettings.setValue("circleNum",fsetting.circleNum);                   //Բ����
    newSettings.setValue("isAngleChecked",fsetting.isAngleChecked);         //�����
    newSettings.setValue("isCircleChecked",fsetting.isCircleChecked);       //Բ�ܼ�
    newSettings.setValue("motorSpeed",fsetting.motorSpeed);                 //ɨ���ٶ�
    newSettings.setValue("intervalTime",fsetting.intervalTime);             //������
    newSettings.setValue("groupTime",fsetting.groupTime);                   //ÿ��̽��ʱ��
    newSettings.endGroup();

    newSettings.beginGroup("Sample_Parameters");
    newSettings.setValue("sampleFreq",fsetting.sampleFreq);                 //����Ƶ��
    newSettings.setValue("triggerLevel",fsetting.triggerLevel);             //������ƽ
    newSettings.setValue("nPointsPreTrigger",fsetting.nPointsPreTrigger);   //Ԥ�����������������ݲ��ṩ����
    newSettings.endGroup();

    newSettings.beginGroup("Realtime_Process");
    newSettings.setValue("nPulsesAcc",fsetting.nPulsesAcc);                 //�������ۼ�������
    newSettings.setValue("nRangeBin",fsetting.nRangeBin);                   //��������
    newSettings.setValue("nPointsPerBin",fsetting.nPointsPerBin);           //�������ڵ���
    newSettings.setValue("overlapRatio",fsetting.overlapRatio);             //�������ص���
    newSettings.setValue("nPointsMirrorWidth",fsetting.nPointsMirrorWidth); //������ٷ�Χ
    newSettings.endGroup();

    newSettings.beginGroup("Vector_Velocity_Inversion");
    newSettings.setValue("nPointsObjFreq", fsetting.nPointsObjFreq);        //���㾶�����ʱĿ��������
    newSettings.setValue("nDirsVectorCal", fsetting.nDirsVectorCal);        //����ʸ�������õ��ľ�����ٸ���
    newSettings.endGroup();

    newSettings.beginGroup("File_Store");
    newSettings.setValue("dataFilePath",fsetting.dataFilePath);             //�ļ�����·��
    newSettings.setValue("autoCreateDateDir",fsetting.autoCreateDateDir);   //�Զ����������ļ���
    newSettings.setValue("nDirsPerFile",fsetting.nDirsPerFile);
    newSettings.endGroup();
}

SOFTWARESETTINGS SettingFile::readSettings()
{
//    qDebug() << userIniFilePath;
    QSettings settings(userIniFilePath, QSettings::IniFormat);
    settings.beginGroup("Laser_Parameters");
    fsetting.laserPulseEnergy = settings.value("laserPulseEnergy",10.0).toDouble(); //��������
    fsetting.laserLocalPower = settings.value("laserLocalPower",1.0).toDouble();    //���⹦��
    fsetting.laserRPF = settings.value("laserRPF",10.0).toDouble();                 //������Ƶ
    fsetting.laserPulseWidth = settings.value("laserPulseWidth",400.0).toInt();     //��������
    fsetting.laserWaveLength = settings.value("laserWaveLength",1.550).toDouble();  //���Ⲩ��
    fsetting.laserAOMFreq = settings.value("laserAOMFreq",120.0).toDouble();        //AOM��Ƶ��
    settings.endGroup();

    settings.beginGroup("Scan_Parameters");
    fsetting.detectMode = settings.value("detectMode",0).toUInt();                  //̽��ģʽ
    fsetting.elevationAngle = settings.value("elevationAngle",70.0).toDouble();     //������
    fsetting.azAngleStart = settings.value("azAngleStart",0.0).toDouble();          //��ʼ��
    fsetting.azAngleStep = settings.value("azAngleStep",45.0).toDouble();           //������
    fsetting.angleNum = settings.value("angleNum",80).toUInt();                     //������
    fsetting.circleNum = settings.value("circleNum",10.0).toDouble();               //Բ����
    fsetting.isAngleChecked = settings.value("isAngleChecked",true).toBool();       //�����
    fsetting.isCircleChecked = settings.value("isCircleChecked",false).toBool();    //Բ�ܼ�
    fsetting.motorSpeed = settings.value("motorSpeed",90.0).toDouble();             //����ٶ�
    fsetting.intervalTime = settings.value("intervalTime",15.0).toDouble();         //�������
    fsetting.groupTime = settings.value("groupTime",3.0).toDouble();                //Բ�ܼ���
    settings.endGroup();

    settings.beginGroup("Sample_Parameters");
    fsetting.sampleFreq = settings.value("sampleFreq",400.0).toDouble();            //����Ƶ��
    fsetting.triggerLevel = settings.value("triggerLevel",1500).toUInt();           //������ƽ
    fsetting.nPointsPreTrigger = settings.value("nPointsPreTrigger",500).toUInt();  //Ԥ�����������������ݲ��ṩ����
    settings.endGroup();

    settings.beginGroup("Realtime_Process");
    fsetting.nPulsesAcc = settings.value("nPulsesAcc",10000).toUInt();              //�������ۼ�������
    fsetting.nRangeBin = settings.value("nRangeBin",14).toUInt();                   //��������
    fsetting.nPointsPerBin = settings.value("nPointsPerBin",250).toUInt();          //�������ڵ���
    fsetting.overlapRatio = settings.value("overlapRatio",0).toUInt();              //������ٷ�Χ
    fsetting.nPointsMirrorWidth = settings.value("nPointsMirrorWidth",250).toUInt();//�������
    settings.endGroup();

    settings.beginGroup("Vector_Velocity_Inversion");
    fsetting.nPointsObjFreq = settings.value("nPointsObjFreq",40).toUInt();         //Ŀ����ٴ�С����
    fsetting.nDirsVectorCal = settings.value("nDirsVectorCal",8).toUInt();          //�ϳ�ʸ�����ٵľ�����ٸ���
    settings.endGroup();

    QString docu_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString dataStorePath = docu_path + "/KnapsackCDL_data/";

    settings.beginGroup("File_Store");
    fsetting.dataFilePath = settings.value("dataFilePath",dataStorePath).toString();  //�ļ�����·��
    fsetting.autoCreateDateDir = settings.value("autoCreateDateDir",true).toBool();   //�Զ�������С�ļ���
    fsetting.nDirsPerFile = settings.value("nDirsPerFile",1000).toUInt();             //���ļ�������
    settings.endGroup();

    return fsetting;
}

void SettingFile::testSettings()
{
    QFileInfo file(defaultIniFilePath);
    if(file.exists() == false)                              //����Ϊ�����ļ�������ʱ��Ĭ������
    {
        userIniFilePath = defaultIniFilePath;
        QString docu_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString dataStorePath = docu_path + "/KnapsackCDL_data";
        QSettings settings(defaultIniFilePath,QSettings::IniFormat);

        settings.beginGroup("Ini_File_Path");
        settings.setValue("userIniFilePath",userIniFilePath);
        settings.endGroup();

        settings.beginGroup("Laser_Parameters");
        settings.setValue("laserPulseEnergy",10.0);             //��������
        settings.setValue("laserLocalPower",1.0);               //���⹦��
        settings.setValue("laserRPF",10.0);                     //������Ƶ
        settings.setValue("laserPulseWidth",400.0);             //��������
        settings.setValue("laserWaveLength",1.55);              //���Ⲩ��
        settings.setValue("laserAOMFreq",120.0);                //AOM��Ƶ��
        settings.endGroup();

        settings.beginGroup("Scan_Parameters");
        settings.setValue("detectMode",0);                      //̽��ģʽ��0����̽��1����̽��2��ʱ̽��
        settings.setValue("elevationAngle",70.0);               //������
        settings.setValue("azAngleStart",0.0);                  //��ʼ��
        settings.setValue("azAngleStep",45.0);                  //������
        settings.setValue("angleNum",80);                       //������
        settings.setValue("circleNum",10.0);                    //Բ����
        settings.setValue("isAngleChecked",true);               //�����
        settings.setValue("isCircleChecked",false);             //Բ�ܼ�
        settings.setValue("motorSpeed",90.0);                   //ɨ���ٶ�
        settings.setValue("intervalTime",15.0);                 //������
        settings.setValue("groupTime",3.0);                     //ÿ��̽��ʱ��
        settings.endGroup();

        settings.beginGroup("Sample_Parameters");
        settings.setValue("sampleFreq",400.0);                  //����Ƶ��
        settings.setValue("triggerLevel",1500);                 //������ƽ
        settings.setValue("nPointsPreTrigger",500);             //Ԥ�����������������ݲ��ṩ����
        settings.endGroup();

        settings.beginGroup("Realtime_Process");
        settings.setValue("nPulsesAcc",10000);                   //�������ۼ�������
        settings.setValue("nRangeBin",14);                       //��������
        settings.setValue("nPointsPerBin",250);                  //�������ڵ���
        settings.setValue("overlapRatio",0);                     //�������ص���
        settings.setValue("nPointsPerBin",250);                  //������ٷ�Χ
        settings.endGroup();

        settings.beginGroup("Vector_Velocity_Inversion");
        settings.setValue("nPointsObjFreq", 40);
        settings.setValue("nDirsVectorCal", 8);
        settings.endGroup();

        settings.beginGroup("File_Store");
        settings.setValue("dataFilePath",dataStorePath);        //�ļ�����·��
        settings.setValue("autoCreateDateDir",true);            //�Զ����������ļ���
        settings.setValue("nDirsPerFile",1000);
        settings.endGroup();
    }
    else {
        QSettings settings(defaultIniFilePath, QSettings::IniFormat);
        settings.beginGroup("Ini_File_Path");
        userIniFilePath = settings.value("userIniFilePath", userIniFilePath).toString();
//        qDebug() << userIniFilePath;
        settings.endGroup();
    }
}

bool SettingFile::isSettingsChanged(const SOFTWARESETTINGS &setting)
{
    SOFTWARESETTINGS dlgsetting = setting;
    if(fsetting.laserPulseEnergy != dlgsetting.laserPulseEnergy)    //��������
        return true;
    if(fsetting.laserLocalPower != dlgsetting.laserLocalPower)      //���⹦��
        return true;
    if(fsetting.laserRPF != dlgsetting.laserRPF)                    //������Ƶ
        return true;
    if(fsetting.laserPulseWidth != dlgsetting.laserPulseWidth)      //������
        return true;
    if(fsetting.laserWaveLength != dlgsetting.laserWaveLength)      //���Ⲩ��
        return true;
    if(fsetting.laserAOMFreq != dlgsetting.laserAOMFreq)            //AOM��Ƶ��
        return true;

    if(fsetting.detectMode != dlgsetting.detectMode)                //̽�ⷽʽ
        return true;
    if(fsetting.elevationAngle != dlgsetting.elevationAngle)        //������
        return true;
    if(fsetting.azAngleStart != dlgsetting.azAngleStart)            //��ʼ��
        return true;
    if(fsetting.azAngleStep != dlgsetting.azAngleStep)              //������
        return true;
    if(fsetting.angleNum != dlgsetting.angleNum)                    //������
        return true;
    if(fsetting.circleNum != dlgsetting.circleNum)                  //Բ����
        return true;
    if(fsetting.azAngleStep != dlgsetting.azAngleStep)
        return true;
    if(fsetting.isCircleChecked != dlgsetting.isCircleChecked)
        return true;
    if(fsetting.motorSpeed != dlgsetting.motorSpeed)                //����ٶ�
        return true;
    if(fsetting.intervalTime != dlgsetting.intervalTime)
        return true;
    if(fsetting.groupTime != dlgsetting.groupTime)
        return true;

    if(fsetting.sampleFreq != dlgsetting.sampleFreq)                //����Ƶ��
        return true;
    if(fsetting.triggerLevel != dlgsetting.triggerLevel)            //������ƽ
        return true;
    if(fsetting.nPointsPreTrigger !=dlgsetting.nPointsPreTrigger)   //Ԥ��������
        return true;

    if(fsetting.nPulsesAcc != dlgsetting.nPulsesAcc)                //�������ۼ�������
        return true;
    if(fsetting.nRangeBin !=dlgsetting.nRangeBin)                   //��������
        return true;
    if(fsetting.nPointsPerBin != dlgsetting.nPointsPerBin)          //�������ڵ���
        return true;
    if(fsetting.nPointsMirrorWidth !=dlgsetting.nPointsMirrorWidth)
        return true;
    if(fsetting.overlapRatio != dlgsetting.overlapRatio)
        return true;

    if(fsetting.nPointsObjFreq != dlgsetting.nPointsObjFreq)         //������ٷ�Χ
        return true;
    if(fsetting.nDirsVectorCal != dlgsetting.nDirsVectorCal)
        return true;

    if(fsetting.dataFilePath != dlgsetting.dataFilePath)             //�ļ�����·��
        return true;
    if(fsetting.autoCreateDateDir != dlgsetting.autoCreateDateDir)   //�Զ����������ļ���
        return true;
    if(fsetting.nDirsPerFile != dlgsetting.nDirsPerFile)             //���ļ�������
        return true;

    return false;
}

void SettingFile::setUserIniFilePath(QString userIniFilePath)
{
    this->userIniFilePath = userIniFilePath;
    QSettings settings(defaultIniFilePath, QSettings::IniFormat);
    settings.beginGroup("Ini_File_Path");
    settings.setValue("userIniFilePath",userIniFilePath);             //�����ļ�·��
    settings.endGroup();
}

QString SettingFile::getUserIniFilePath()
{
    return userIniFilePath;
}

QString SettingFile::getDefaultIniFilePath()
{
    return defaultIniFilePath;
}
