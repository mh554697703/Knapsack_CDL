#include "settingfile.h"

SettingFile::SettingFile()
{
    defaultIniFilePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    defaultIniFilePath = defaultIniFilePath + "/Default.ini";
    testSettings();
//    qDebug() << defaultIniFilePath;
//    readSettings();
}

void SettingFile::writeSettings(const SOFTWARESETTINGS &setting)      //写入文件
{
    fsetting = setting;
//    qDebug() << userIniFilePath;
    QSettings newSettings(userIniFilePath, QSettings::IniFormat);
    newSettings.beginGroup("Laser_Parameters");
    newSettings.setValue("laserPulseEnergy",fsetting.laserPulseEnergy);     //激光能量
    newSettings.setValue("laserLocalPower",fsetting.laserLocalPower);       //激光功率
    newSettings.setValue("laserRPF",fsetting.laserRPF);                     //激光重频
    newSettings.setValue("laserPulseWidth",fsetting.laserPulseWidth);       //激光脉宽
    newSettings.setValue("laserWaveLength",fsetting.laserWaveLength);       //激光波长
    newSettings.setValue("laserAOMFreq",fsetting.laserAOMFreq);             //AOM移频量
    newSettings.endGroup();

    newSettings.beginGroup("Scan_Parameters");
    newSettings.setValue("detectMode",fsetting.detectMode);                 //探测模式：0持续探测1单组探测2定时探测
    newSettings.setValue("elevationAngle",fsetting.elevationAngle);         //俯仰角
    newSettings.setValue("azAngleStart",fsetting.azAngleStart);             //起始角
    newSettings.setValue("azAngleStep",fsetting.azAngleStep);               //步进角
    newSettings.setValue("angleNum",fsetting.angleNum);                     //方向数
    newSettings.setValue("circleNum",fsetting.circleNum);                   //圆周数
    newSettings.setValue("isAngleChecked",fsetting.isAngleChecked);         //方向键
    newSettings.setValue("isCircleChecked",fsetting.isCircleChecked);       //圆周键
    newSettings.setValue("motorSpeed",fsetting.motorSpeed);                 //扫描速度
    newSettings.setValue("intervalTime",fsetting.intervalTime);             //分组间隔
    newSettings.setValue("groupTime",fsetting.groupTime);                   //每组探测时间
    newSettings.endGroup();

    newSettings.beginGroup("Sample_Parameters");
    newSettings.setValue("sampleFreq",fsetting.sampleFreq);                 //采样频率
    newSettings.setValue("triggerLevel",fsetting.triggerLevel);             //触发电平
    newSettings.setValue("nPointsPreTrigger",fsetting.nPointsPreTrigger);   //预触发点数，保留，暂不提供设置
    newSettings.endGroup();

    newSettings.beginGroup("Realtime_Process");
    newSettings.setValue("nPulsesAcc",fsetting.nPulsesAcc);                 //单方向累加脉冲数
    newSettings.setValue("nRangeBin",fsetting.nRangeBin);                   //距离门数
    newSettings.setValue("nPointsPerBin",fsetting.nPointsPerBin);           //距离门内点数
    newSettings.setValue("overlapRatio",fsetting.overlapRatio);             //距离门重叠率
    newSettings.setValue("nPointsMirrorWidth",fsetting.nPointsMirrorWidth); //径向风速范围
    newSettings.endGroup();

    newSettings.beginGroup("Vector_Velocity_Inversion");
    newSettings.setValue("nPointsObjFreq", fsetting.nPointsObjFreq);        //计算径向风速时目标带宽点数
    newSettings.setValue("nDirsVectorCal", fsetting.nDirsVectorCal);        //计算矢量风速用到的径向风速个数
    newSettings.endGroup();

    newSettings.beginGroup("File_Store");
    newSettings.setValue("dataFilePath",fsetting.dataFilePath);             //文件保存路径
    newSettings.setValue("autoCreateDateDir",fsetting.autoCreateDateDir);   //自动创建日期文件夹
    newSettings.setValue("nDirsPerFile",fsetting.nDirsPerFile);
    newSettings.endGroup();
}

SOFTWARESETTINGS SettingFile::readSettings()
{
//    qDebug() << userIniFilePath;
    QSettings settings(userIniFilePath, QSettings::IniFormat);
    settings.beginGroup("Laser_Parameters");
    fsetting.laserPulseEnergy = settings.value("laserPulseEnergy",10.0).toDouble(); //激光能量
    fsetting.laserLocalPower = settings.value("laserLocalPower",1.0).toDouble();    //激光功率
    fsetting.laserRPF = settings.value("laserRPF",10.0).toDouble();                 //激光重频
    fsetting.laserPulseWidth = settings.value("laserPulseWidth",400.0).toInt();     //激光脉宽
    fsetting.laserWaveLength = settings.value("laserWaveLength",1.550).toDouble();  //激光波长
    fsetting.laserAOMFreq = settings.value("laserAOMFreq",120.0).toDouble();        //AOM移频量
    settings.endGroup();

    settings.beginGroup("Scan_Parameters");
    fsetting.detectMode = settings.value("detectMode",0).toUInt();                  //探测模式
    fsetting.elevationAngle = settings.value("elevationAngle",70.0).toDouble();     //俯仰角
    fsetting.azAngleStart = settings.value("azAngleStart",0.0).toDouble();          //起始角
    fsetting.azAngleStep = settings.value("azAngleStep",45.0).toDouble();           //步进角
    fsetting.angleNum = settings.value("angleNum",80).toUInt();                     //方向数
    fsetting.circleNum = settings.value("circleNum",10.0).toDouble();               //圆周数
    fsetting.isAngleChecked = settings.value("isAngleChecked",true).toBool();       //方向键
    fsetting.isCircleChecked = settings.value("isCircleChecked",false).toBool();    //圆周键
    fsetting.motorSpeed = settings.value("motorSpeed",90.0).toDouble();             //电机速度
    fsetting.intervalTime = settings.value("intervalTime",15.0).toDouble();         //方向间间隔
    fsetting.groupTime = settings.value("groupTime",3.0).toDouble();                //圆周间间隔
    settings.endGroup();

    settings.beginGroup("Sample_Parameters");
    fsetting.sampleFreq = settings.value("sampleFreq",400.0).toDouble();            //采样频率
    fsetting.triggerLevel = settings.value("triggerLevel",1500).toUInt();           //触发电平
    fsetting.nPointsPreTrigger = settings.value("nPointsPreTrigger",500).toUInt();  //预触发点数，保留，暂不提供设置
    settings.endGroup();

    settings.beginGroup("Realtime_Process");
    fsetting.nPulsesAcc = settings.value("nPulsesAcc",10000).toUInt();              //单方向累加脉冲数
    fsetting.nRangeBin = settings.value("nRangeBin",14).toUInt();                   //距离门数
    fsetting.nPointsPerBin = settings.value("nPointsPerBin",250).toUInt();          //距离门内点数
    fsetting.overlapRatio = settings.value("overlapRatio",0).toUInt();              //径向风速范围
    fsetting.nPointsMirrorWidth = settings.value("nPointsMirrorWidth",250).toUInt();//镜面点数
    settings.endGroup();

    settings.beginGroup("Vector_Velocity_Inversion");
    fsetting.nPointsObjFreq = settings.value("nPointsObjFreq",40).toUInt();         //目标风速大小点数
    fsetting.nDirsVectorCal = settings.value("nDirsVectorCal",8).toUInt();          //合成矢量风速的径向风速个数
    settings.endGroup();

    QString docu_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString dataStorePath = docu_path + "/KnapsackCDL_data/";

    settings.beginGroup("File_Store");
    fsetting.dataFilePath = settings.value("dataFilePath",dataStorePath).toString();  //文件保存路径
    fsetting.autoCreateDateDir = settings.value("autoCreateDateDir",true).toBool();   //自动创建最小文件夹
    fsetting.nDirsPerFile = settings.value("nDirsPerFile",1000).toUInt();             //单文件方向数
    settings.endGroup();

    return fsetting;
}

void SettingFile::testSettings()
{
    QFileInfo file(defaultIniFilePath);
    if(file.exists() == false)                              //以下为配置文件不存在时的默认配置
    {
        userIniFilePath = defaultIniFilePath;
        QString docu_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString dataStorePath = docu_path + "/KnapsackCDL_data";
        QSettings settings(defaultIniFilePath,QSettings::IniFormat);

        settings.beginGroup("Ini_File_Path");
        settings.setValue("userIniFilePath",userIniFilePath);
        settings.endGroup();

        settings.beginGroup("Laser_Parameters");
        settings.setValue("laserPulseEnergy",10.0);             //激光能量
        settings.setValue("laserLocalPower",1.0);               //激光功率
        settings.setValue("laserRPF",10.0);                     //激光重频
        settings.setValue("laserPulseWidth",400.0);             //激光脉宽
        settings.setValue("laserWaveLength",1.55);              //激光波长
        settings.setValue("laserAOMFreq",120.0);                //AOM移频量
        settings.endGroup();

        settings.beginGroup("Scan_Parameters");
        settings.setValue("detectMode",0);                      //探测模式：0持续探测1单组探测2定时探测
        settings.setValue("elevationAngle",70.0);               //俯仰角
        settings.setValue("azAngleStart",0.0);                  //起始角
        settings.setValue("azAngleStep",45.0);                  //步进角
        settings.setValue("angleNum",80);                       //方向数
        settings.setValue("circleNum",10.0);                    //圆周数
        settings.setValue("isAngleChecked",true);               //方向键
        settings.setValue("isCircleChecked",false);             //圆周键
        settings.setValue("motorSpeed",90.0);                   //扫描速度
        settings.setValue("intervalTime",15.0);                 //分组间隔
        settings.setValue("groupTime",3.0);                     //每组探测时间
        settings.endGroup();

        settings.beginGroup("Sample_Parameters");
        settings.setValue("sampleFreq",400.0);                  //采样频率
        settings.setValue("triggerLevel",1500);                 //触发电平
        settings.setValue("nPointsPreTrigger",500);             //预触发点数，保留，暂不提供设置
        settings.endGroup();

        settings.beginGroup("Realtime_Process");
        settings.setValue("nPulsesAcc",10000);                   //单方向累加脉冲数
        settings.setValue("nRangeBin",14);                       //距离门数
        settings.setValue("nPointsPerBin",250);                  //距离门内点数
        settings.setValue("overlapRatio",0);                     //距离门重叠率
        settings.setValue("nPointsPerBin",250);                  //径向风速范围
        settings.endGroup();

        settings.beginGroup("Vector_Velocity_Inversion");
        settings.setValue("nPointsObjFreq", 40);
        settings.setValue("nDirsVectorCal", 8);
        settings.endGroup();

        settings.beginGroup("File_Store");
        settings.setValue("dataFilePath",dataStorePath);        //文件保存路径
        settings.setValue("autoCreateDateDir",true);            //自动创建日期文件夹
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
    if(fsetting.laserPulseEnergy != dlgsetting.laserPulseEnergy)    //激光能量
        return true;
    if(fsetting.laserLocalPower != dlgsetting.laserLocalPower)      //激光功率
        return true;
    if(fsetting.laserRPF != dlgsetting.laserRPF)                    //激光重频
        return true;
    if(fsetting.laserPulseWidth != dlgsetting.laserPulseWidth)      //脉冲宽度
        return true;
    if(fsetting.laserWaveLength != dlgsetting.laserWaveLength)      //激光波长
        return true;
    if(fsetting.laserAOMFreq != dlgsetting.laserAOMFreq)            //AOM移频量
        return true;

    if(fsetting.detectMode != dlgsetting.detectMode)                //探测方式
        return true;
    if(fsetting.elevationAngle != dlgsetting.elevationAngle)        //俯仰角
        return true;
    if(fsetting.azAngleStart != dlgsetting.azAngleStart)            //起始角
        return true;
    if(fsetting.azAngleStep != dlgsetting.azAngleStep)              //步进角
        return true;
    if(fsetting.angleNum != dlgsetting.angleNum)                    //方向数
        return true;
    if(fsetting.circleNum != dlgsetting.circleNum)                  //圆周数
        return true;
    if(fsetting.azAngleStep != dlgsetting.azAngleStep)
        return true;
    if(fsetting.isCircleChecked != dlgsetting.isCircleChecked)
        return true;
    if(fsetting.motorSpeed != dlgsetting.motorSpeed)                //电机速度
        return true;
    if(fsetting.intervalTime != dlgsetting.intervalTime)
        return true;
    if(fsetting.groupTime != dlgsetting.groupTime)
        return true;

    if(fsetting.sampleFreq != dlgsetting.sampleFreq)                //采样频率
        return true;
    if(fsetting.triggerLevel != dlgsetting.triggerLevel)            //触发电平
        return true;
    if(fsetting.nPointsPreTrigger !=dlgsetting.nPointsPreTrigger)   //预触发点数
        return true;

    if(fsetting.nPulsesAcc != dlgsetting.nPulsesAcc)                //单方向累加脉冲数
        return true;
    if(fsetting.nRangeBin !=dlgsetting.nRangeBin)                   //距离门数
        return true;
    if(fsetting.nPointsPerBin != dlgsetting.nPointsPerBin)          //距离门内点数
        return true;
    if(fsetting.nPointsMirrorWidth !=dlgsetting.nPointsMirrorWidth)
        return true;
    if(fsetting.overlapRatio != dlgsetting.overlapRatio)
        return true;

    if(fsetting.nPointsObjFreq != dlgsetting.nPointsObjFreq)         //径向风速范围
        return true;
    if(fsetting.nDirsVectorCal != dlgsetting.nDirsVectorCal)
        return true;

    if(fsetting.dataFilePath != dlgsetting.dataFilePath)             //文件保存路径
        return true;
    if(fsetting.autoCreateDateDir != dlgsetting.autoCreateDateDir)   //自动创建日期文件夹
        return true;
    if(fsetting.nDirsPerFile != dlgsetting.nDirsPerFile)             //单文件方向数
        return true;

    return false;
}

void SettingFile::setUserIniFilePath(QString userIniFilePath)
{
    this->userIniFilePath = userIniFilePath;
    QSettings settings(defaultIniFilePath, QSettings::IniFormat);
    settings.beginGroup("Ini_File_Path");
    settings.setValue("userIniFilePath",userIniFilePath);             //设置文件路径
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
