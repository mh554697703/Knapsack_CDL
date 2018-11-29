#include "devicescontrol.h"
#include <QtMath>

# define method 1

DevicesControl::DevicesControl(QObject *parent) : QObject(parent)
{
    CM_Time = new QTime();
    CS_Time = new QTime();
    workTime = new QTime();
    Motor = new motor();
    connect(&Compass, &compass::compassAngle, this, &DevicesControl::pulse_laser_opened_fcn);
    connect(&Compass, &compass::compassAngle, Motor,&motor::prepare);
    connect(Motor, &motor::motorPrepareOk, this, &DevicesControl::readyToMove);
    connect(Motor, &motor::positionArrived, this, &DevicesControl::SetMotorFlag);

    connect(&LaserSeed, &laserSeed::seedOpenReady, &LaserPulse,&laserPulse::beginPulseLaser);
    connect(&LaserPulse, &laserPulse::laserPulseOk, &LaserSeed,&laserSeed::checkLaser);
    connect(&LaserSeed, &laserSeed::laserSeedOk, this, &DevicesControl::laserReady);

    connect(&Compass,&compass::compassError,this,&DevicesControl::compassError);
    connect(Motor, &motor::motorError, this, &DevicesControl::motorError);
    connect(&LaserSeed, &laserSeed::laserSeedError, this, &DevicesControl::laserseedError);
    connect(&LaserPulse, &laserPulse::laserPulseError, this, &DevicesControl::laserpulseError);
    connect(&adq,&ADQ214::adqError,this,&DevicesControl::adqError);

    connect(Motor, &motor::motorClosed, this, &DevicesControl::motorCloseOk);
    connect(&LaserPulse, &laserPulse::pulseCloseReady, &LaserSeed, &laserSeed::closeSeedLaser);
    connect(&LaserSeed, &laserSeed::laserColseRight, this, &DevicesControl::laserCloseOk);
    connect(&LaserSeed, &laserSeed::laserColseRight, this, &DevicesControl::detectionFinished);

    Motorthread = new QThread;                 //motor进入独立线程
    Motor->moveToThread(Motorthread);
    Motorthread->start();
}

void DevicesControl::startAction(SOFTWARESETTINGS settings)
{
    qDebug() << "start action";
    this->mysetting = settings;
    if(mysetting.overlapRatio>0)
        nRB_ovlp = mysetting.nRangeBin*2-1;
    else
        nRB_ovlp = mysetting.nRangeBin;
    motorInPosition = false;
    LaserReady = false;
    Compass.read();
    CM_Time->start();
    LaserSeed.beginSeedLaser(mysetting.laserLocalPower,mysetting.laserPulseEnergy);
    Motor->set_Motor_azAngleStep(mysetting.azAngleStep);
}

void DevicesControl::stopAction()
{
    //待完善
    qDebug() << "stop action!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    Stop_now = true;
}

void DevicesControl::checkMotor()             //师兄之前定义的函数，名字看起来是有用的，但实际没用上，暂时保留一下
{
    QSerialPort my_serial;
    my_serial.setPortName(MotorComPort);
    if(!my_serial.open(QIODevice::ReadWrite))
    {
        qDebug()<<"motor not open";
    }

    my_serial.setBaudRate(QSerialPort::Baud19200);
    my_serial.setDataBits(QSerialPort::Data8);
    my_serial.setStopBits(QSerialPort::OneStop);
    my_serial.setFlowControl(QSerialPort::NoFlowControl);
    QString test("VR;");
    QByteArray testData = test.toLocal8Bit();
    my_serial.write(testData);
    if(my_serial.waitForBytesWritten(15))
    {
        if(my_serial.waitForReadyRead(30))
        {
            QByteArray testResponse = my_serial.readAll();
            while(my_serial.waitForReadyRead(10))
                testResponse += my_serial.readAll();
            QString response(testResponse);
            if(response.left(10) == "VR;Whistle")
            {
                qDebug()<<"motor ok";
            }
            else
            {qDebug()<<"motor error";}
        }
    }
}

//void DevicesControl::motorErrorSolve()
//{
//    qDebug()<<"motor already stop";
//}

void DevicesControl::readyToMove()
{
    qDebug() <<"motor prepare ok";
}

void DevicesControl::SetMotorFlag(const double &s)
{
    currentMotorAngle = s;
    motorInPosition = true;
    int moveUseTime = CM_Time->restart();
    qDebug() <<"motorTime = "<<moveUseTime;
    qDebug() <<"motor move ok";
}

void DevicesControl::laserReady()
{
    LaserReady = true;
    LaserCloseOk = false;
    qDebug() <<"laser ready";
}
// 罗盘读好了，电机转到位了，激光器也都打开了
// 在这里，完成所有的准备工作

void DevicesControl::motorCloseOk()
{
    MotorCloseOk = true;
}

void DevicesControl::laserCloseOk()
{
    LaserCloseOk = true;
}

void DevicesControl::pulse_laser_opened_fcn(const double &s)
{
    qDebug() << "begin initializing!!!";
    headAngle = s;
    capture_counter = 0;        // 探测方向计数器置零

    Stop_now = false;
    if( mysetting.detectMode == 2)
    {
        Start_Time = QDateTime::currentDateTimeUtc();
    }
    State = WaitAndWork;
    keepWorking = true;
    Generate_freqAxis();
    SaveSpec_FileHead();        //建立新文件，写入文件头
    SaveVelo_FileHead();        //建立新文件，写入文件头
    adq.Transfer_Settings(mysetting);
    Init_Buffers();
    azimuthAngle = VectorXd::Zero(mysetting.nDirsVectorCal);
    losVelocityMat = MatrixXd::Zero(nRB_ovlp, mysetting.nDirsVectorCal);

    this->beginWork();
}

void DevicesControl::beginWork()
{
    qDebug() << "begin working!!!!!!!";
    QDateTime currentTime;
    qint64 dt;
  while (keepWorking)
  {
    switch (State)
    {
    case WaitAndWork:
        if(motorInPosition && LaserReady)
        {
            if(Stop_now)                     //如果停止按钮按下
            {
                Motor->motorQuit();
                LaserPulse.closePulseLaser();
                State = WaitAndStop;
            }
            else
            {
                switch (mysetting.detectMode)
                {
                case 0:                 //单组探测
                    //判断探测方向数
                    if(capture_counter == mysetting.angleNum)
                    {
                        Stop_now = true;
                        Motor->motorQuit();
                        LaserPulse.closePulseLaser();
                        State = WaitAndStop;
                    }
                    break;

                case 1:                 //持续探测
                    break;

                case 2:                 //定时探测
                    // 判断否达到待机条件
                    dt = currentTime.msecsTo(Start_Time);
                    if(dt > qint64(mysetting.groupTime*60*1000))  // *60s*1000ms 如果达到待机时间
                    {
                        Stop_now = true;
                        Motor->motorQuit();
                        LaserPulse.closePulseLaser();
                        State = WaitAndStop;
                    }
                    break;
                default:
                    break;
                }
               if(!Stop_now)
               {
                   int workUseTime = workTime->restart();
                   qDebug()<<"workUseTime = "<<workUseTime;
                   qDebug() << "capture start!!!";
                   CM_Time->restart();
                bool status = adq.Start_Capture();                 //开始采集
                int CaptureUseTime = CM_Time->restart();
                 qDebug() << "CaptureUseTime = " <<CaptureUseTime;
                 if(status == false)
                 {
                 qDebug() << "ADQ failed";
                 return;
                 }
                 CaptureTime = QDateTime::currentDateTimeUtc();     //记录当前时间，将来写入文件
                 LaserReady = false;
                 motorInPosition = false;
                 LaserPulse.checkLaser();                           //检查激光器
                 CM_Time->restart();
                 Motor->moveRelative(-mysetting.azAngleStep);        //电机转至下一个方向

            //以下为风速计算和存储
                 CS_Time->start();
                 LOSVelocityCal(nRB_ovlp+2, nFFT_half,
                                20, mysetting.laserWaveLength,
                                freqAxis, adq.get_PSD_double());     //径向风速计算

                 //****矢量风速合成
                 qDebug() << "start vector velocity";
                 VectorXd tempAzAngle = azimuthAngle.tail(mysetting.nDirsVectorCal-1);
                 azimuthAngle.head(mysetting.nDirsVectorCal-1) = tempAzAngle;
                 azimuthAngle(mysetting.nDirsVectorCal-1) = currentMotorAngle;

                 for (uint i=0; i<nRB_ovlp; i++) {
                     for (uint j=1; j<mysetting.nDirsVectorCal; j++) {
                         losVelocityMat(i,j-1) = losVelocityMat(i,j);
                     }
                 }
                 for (uint i=0; i<nRB_ovlp; i++) {
                     losVelocityMat(i,mysetting.nDirsVectorCal-1) = losVelocity[i];
                 }

                 if (capture_counter > mysetting.nDirsVectorCal)
                 {
                     if(method==1){
                         //滤波正弦波拟合
                         double *azAngle = new double[azimuthAngle.rows()];
                         for (int i = 0; i < azimuthAngle.rows(); i++) {
                             azAngle[i] = azimuthAngle(i);
                         }
                         double sigama2 = 4;

                         if (hVelocity != nullptr) {
                             delete [] hVelocity;
                         }
                         if (hAngle != nullptr) {
                             delete [] hAngle;
                         }
                         if (vVelocity != nullptr) {
                             delete [] vVelocity;
                         }
                         hVelocity = new double [nRB_ovlp];
                         hAngle = new double [nRB_ovlp];
                         vVelocity = new double [nRB_ovlp];

                         for(uint i=0;i<nRB_ovlp;i++)
                         {
                             double *losVelocityPerHeight=new double [mysetting.nDirsVectorCal];
                             for(uint j=0;j<mysetting.nDirsVectorCal;j++)
                             {
                                 losVelocityPerHeight[j]=losVelocityMat(i,j); //每个高度层各个方向的径向风速
                             }
                             vectorVelocity = fswf.getVelocity(mysetting.nDirsVectorCal,sigama2,mysetting.elevationAngle,azAngle,losVelocityPerHeight);
                             //根据返回的径向风速依次得到水平风速、水平风向、垂直风速
                             hVelocity[i] = qSqrt(*(vectorVelocity+1)**(vectorVelocity+1) +
                                                *(vectorVelocity+2)**(vectorVelocity+2));
                             hAngle[i] = qRadiansToDegrees(qAtan2(*(vectorVelocity+2), *(vectorVelocity+1)))+180.0;
//                             hAngle[i] = 0.0-qRadiansToDegrees(qAtan2(*(vectorVelocity+2), *(vectorVelocity+1)));
//                             if(hAngle[i] < 0) {
//                                 hAngle[i] = hAngle[i] + 360.0;
//                             }
                             vVelocity[i] = *vectorVelocity;
                             delete []losVelocityPerHeight;
                         }
                         delete []azAngle;
                     }
                     else{
                         //直接正弦波拟合
                         std::cout << "azimuthAngle = " << azimuthAngle << std::endl;
                         DSWF dswf(mysetting.elevationAngle,azimuthAngle,mysetting.nDirsVectorCal,nRB_ovlp,losVelocityMat);
                         hVelocity = dswf.getHVelocity();
                         hAngle = dswf.getHAngle();
                         vVelocity = dswf.getVVelocity();
                     }
                     int calculateUseTime = CS_Time->restart();
                     qDebug()<<"calculateUseTime = "<<calculateUseTime;
                     SaveVelo_AddData();     // 存储矢量风速到文件
                     //更新显示
                     emit hVelocityReady(hVelocity,nRB_ovlp);
                     emit hAngleReady(hAngle,nRB_ovlp);
                     emit vVelocityReady(vVelocity,nRB_ovlp);
                 }

                 SaveSpec_AddData();         //存储功率谱到文件
                 int saveUseTime = CS_Time->restart();
                 qDebug()<<"saveUseTime = "<<saveUseTime;

                 capture_counter++;
                 qDebug() << "capture_counter = " << capture_counter;
                 if(mysetting.detectMode == 1 && capture_counter == mysetting.nDirsPerFile)  //达到单文件方向数
                 {
                     SaveSpec_FileHead();
                     SaveVelo_FileHead();
                 }
               }
            }
        }
        break;

    case WaitAndStop:
        if(LaserCloseOk && MotorCloseOk)               //判断是否都关上了
        {
            qDebug() << "all close ok!!!";
            if(mysetting.detectMode == 2)              //若为定时探测
            {
                State = Standby;
            }
            else
            {
                keepWorking = false;                    //在此处结束while循环，但之后要干什么呢？？
                qDebug() << "working end!!!";
            }
        }
        break;

    case Standby:
        currentTime = QDateTime::currentDateTimeUtc();
        dt = currentTime.msecsTo(Start_Time);
        if(dt > qint64(mysetting.intervalTime*60*1000))  // *60s? *1000ms?      //如果达到启动时间
        {
            Start_Time = Start_Time.addMSecs(mysetting.intervalTime*60*1000);   //更新开始时间，为了下次计时
            if (mysetting.azAngleStep != 0)
            {
                capture_counter = 0;        // 探测方向计数器置零,是否需要？
                LaserReady = false;
                motorInPosition = false;
                Stop_now = false;

                Motor->prepare(headAngle);
                LaserSeed.beginSeedLaser(mysetting.laserLocalPower,mysetting.laserPulseEnergy);

                SaveSpec_FileHead();        //建立新文件，写入文件头
                SaveVelo_FileHead();        //建立新文件，写入文件头
                State = WaitAndWork;

            }
        }
        break;

    default:
        break;
    }
    mySleep(100);                   //延时100毫秒
  }
}

void DevicesControl::mySleep(unsigned int msec)
{
    QTime delayTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < delayTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,50);
    }
}

// 生成频率坐标轴，用于计算径向风速，并会被写入到功率谱的记录文件
void DevicesControl::Generate_freqAxis()
{
    for (int i = 0; i < nFFT_half; i++)  {
        freqAxis[i] = mysetting.sampleFreq/2.0*(i+1)/nFFT_half;
        //        qDebug() << freqAxis[i];
    }
}

// 计算径向风速
void DevicesControl::LOSVelocityCal(const unsigned int heightNum, const uint totalSpecPoints, const uint objSpecPoints, const double lambda, const double *freqAxis, const double *specData)
{
    for (uint k = 0; k < totalSpecPoints; k++) {
        aomSpec[k] = specData[totalSpecPoints+k] - specData[k];
        for (uint l = 0; l < heightNum - 2; l++){
            specArray[l*totalSpecPoints+k] = specData[totalSpecPoints*(l+2) + k] - specData[k];
        }
    }
    uint aomIndex = 0;
    double temp = aomSpec[0];
    for (uint k = 1; k < totalSpecPoints; k++) {
        if (aomSpec[k] > temp) {
            temp = aomSpec[k];
            aomIndex = k;
        }
    }

    uint startIndex = aomIndex - objSpecPoints;  // 目标风速区间起点
    uint endIndex = aomIndex + objSpecPoints;    // 目标风速区间终点

    uint *losVelocityIndex = new uint[heightNum - 2];
    temp = 0;
    for (uint l = 0; l < heightNum -2; l++) {
        losVelocityIndex[l] = startIndex;
        temp = specArray[l*totalSpecPoints+ startIndex];
        for (uint k = startIndex + 1; k <= endIndex; k++) {
            if (specArray[l*totalSpecPoints+ k] >temp) {
                temp = specArray[l*totalSpecPoints+ k];
                losVelocityIndex[l] = k;
            }
        }
    }
    memset(losVelocity, 0, sizeof(double)*(heightNum-2));
    for(uint i=0; i<heightNum-2; i++) {
        losVelocity[i] = -(freqAxis[losVelocityIndex[i]] - freqAxis[aomIndex])*lambda/2;
    }
}

// 初始化各个数据缓冲区，申请内存空间
void DevicesControl::Init_Buffers()
{
    losVelocity = new double[nRB_ovlp];
    aomSpec = new double[nFFT_half];
    specArray = new double[nRB_ovlp * nFFT_half];
    adq.Init_Buffers();
}

// 保存矢量风速反演结果——文件头
void DevicesControl::SaveVelo_FileHead()
{
    Create_DataFolder();
    VeloFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").Velo";
    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ VeloFileName +").Velo";
    //    qDebug()<<"VeloFileName = "<<VeloFileName;
    QFile outputVelo(VeloFileName);

    if(outputVelo.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate表示将原文件内容清空
    {
        QTextStream VeloFile(&outputVelo);
        VeloFile << "Knapsack Coherent Doppler Lidar Velocity Vector" << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                                // =分隔符
        VeloFile << endl<< "Data File Version: 1.0" << endl;          // 文件类型版本
        VeloFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        VeloFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //激光参数
        VeloFile << "LaserMode:        ";
        if(/* DISABLES CODE */ (1))
        {
            VeloFile << "Pulse" << endl;		//脉冲探测（true）or连续探测（false） bool
            VeloFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //激光能量，单位μJ，连续模式下为0
        }
        else
        {
            VeloFile << "Continuous" << endl;	//脉冲探测（true）or连续探测（false） bool
            VeloFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl; //激光功率，单位mW，脉冲模式下为0
        }
        VeloFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;			//激光频率
        VeloFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;     //脉冲宽度
        VeloFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;     //激光波长
        VeloFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;        //AOM移频量

        //扫描参数
        VeloFile << "detectMode:       " ;      //探测方式：0持续探测1单组探测2定时探测
        switch (mysetting.detectMode) {
        case 1:                 //持续探测
            VeloFile << "NonStop" << endl;
            break;
        case 0:                 //单组探测
            VeloFile << "SingleGroup" << endl;
            break;
        case 2:                 //定时探测
            VeloFile << "scheduled" << endl;
            break;
        }

        VeloFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//俯仰角
        VeloFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//起始角
        VeloFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;	    //步进角
        VeloFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//方向数
        VeloFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//定时探测间隔，单位：分钟
        VeloFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//定时探测单组时间，单位：分钟

        //采样参数
        VeloFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //采样频率
        VeloFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //触发电平
        VeloFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //预触发点数，保留，暂不提供设置

        //实时处理参数
        VeloFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //单方向累加脉冲数
        VeloFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //距离门数
        VeloFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //距离门内点数

        VeloFile << "Height Axis:      ";
        QString str= "";
        CalHeightsValues();
        for(uint i=0;i<nRB_ovlp;i++)
            str = str + QString::number(Height_values[i],'f', 2) + " ";
        VeloFile << str << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                    // =分隔符
        VeloFile << endl;
        outputVelo.close();
        //        qDebug() << "Velofile Header added!";
    }
}

// 保存功率谱计算结果——文件头
void DevicesControl::SaveSpec_FileHead()
{
    Create_DataFolder();
    SpecFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").spec";
    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
    //    qDebug()<<"SpecFileName = "<<SpecFileName;
    QFile outputSpec(SpecFileName);

    if(outputSpec.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate表示将原文件内容清空
    {
        QTextStream specFile(&outputSpec);
        specFile << "Knapsack Coherent Doppler Lidar Original Spectrum" << endl;
        for(int i=0;i<60;i++)
            specFile << "=";                                // =分隔符
        specFile << endl<< "Data File Version: 1.0" << endl;          // 文件类型版本
        specFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        specFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //激光参数
        specFile << "LaserMode:        ";
        if(/* DISABLES CODE */ (1))
        {
            specFile << "Pulse" << endl;		//脉冲探测（true）or连续探测（false） bool
            specFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //激光能量，单位μJ，连续模式下为0
        }
        else
        {
            specFile << "Continuous" << endl;	//脉冲探测（true）or连续探测（false） bool
            specFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl;     //激光功率，单位mW，脉冲模式下为0
        }
        specFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;                //激光频率
        specFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;         //脉冲宽度
        specFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;         //激光波长
        specFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;            //AOM移频量

        //扫描参数
        specFile << "detectMode:       " ;      //探测方式：0持续探测1单组探测2定时探测
        switch (mysetting.detectMode) {
        case 0:                 //单组探测
            specFile << "SingleGroup" << endl;
            break;
        case 1:                 //持续探测
            specFile << "NonStop" << endl;
            break;
        case 2:                 //定时探测
            specFile << "scheduled" << endl;
            break;
        }

        specFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//俯仰角
        specFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//起始角
        specFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;     //步进角
        specFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//方向数
        specFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//定时探测间隔，单位：分钟
        specFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//定时探测单组时间，单位：分钟

        //采样参数
        specFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //采样频率
        specFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //触发电平
        specFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //预触发点数，保留，暂不提供设置

        //实时处理参数
        specFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //单方向累加脉冲数
        specFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //距离门数
        specFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //距离门内点数

        specFile << "Frequency Axis:   ";
        QString str = "";
        for(int i=0;i<nFFT_half;i++)
            str = str + QString::number(freqAxis[i],'f', 2) + " ";
        specFile << str << endl;

        specFile << "Height Axis:      ";
        CalHeightsValues();
        str = "";
        for(uint i=0;i<nRB_ovlp;i++)
            str = str + QString::number(Height_values[i],'f', 2) + " ";
        specFile << str << endl;

        specFile << "Spectrum Point Size: "<< sizeof(quint64) <<  " Bytes int" << endl;

        for(int i=0;i<60;i++)
            specFile << "=";                    // =分隔符
        specFile << endl;
        outputSpec.close();
        //        qDebug() << "Specfile Header added!";
    }
}

void DevicesControl::SaveSpec_AddData()
{
    QFile outputSpec(SpecFileName);
    if(outputSpec.open(QFile::WriteOnly|QIODevice::Append))       //追加
    {
        QDataStream specFile(&outputSpec);
        quint64 CTms = CaptureTime.toMSecsSinceEpoch();
        specFile.writeRawData((char*)&CTms,sizeof(quint64));
        specFile.writeRawData((char*)&currentMotorAngle,sizeof(double));
        specFile.writeRawData((char*)adq.get_PSD_Union(), (nRB_ovlp+2) * nFFT_half*sizeof(quint64));
        outputSpec.close();
    }
}

void DevicesControl::SaveVelo_AddData()
{qDebug()<<"currentMotorAngle"<<currentMotorAngle;
    QFile outputVelo(VeloFileName);
    if(outputVelo.open(QFile::WriteOnly|QIODevice::Append))       //追加
    {
        QDataStream veloFile(&outputVelo);
        qint64 CTms = CaptureTime.toMSecsSinceEpoch();
        veloFile.writeRawData((char*)&CTms,sizeof(quint64));
        veloFile.writeRawData((char*)&currentMotorAngle,sizeof(double));

        veloFile.writeRawData((char*)hVelocity, nRB_ovlp * sizeof(double));
        veloFile.writeRawData((char*)hAngle, nRB_ovlp * sizeof(double));
        veloFile.writeRawData((char*)vVelocity, nRB_ovlp * sizeof(double));
        outputVelo.close();
    }
}

void DevicesControl::Create_DataFolder()
{
    QDir mypath;
    if(!mypath.exists(mysetting.dataFilePath))		//如果文件夹不存在，创建文件夹
        mypath.mkpath(mysetting.dataFilePath);
}

// 计算各个高度层的高度值，用于写入到结果文件
void DevicesControl::CalHeightsValues()
{
    //垂直向 最小探测距离 minDetectRange
    double resol = lightSpeed/mysetting.sampleFreq/1000000/2;        //单采样点的径向分辨率
    double minDetectRange = resol*(mysetting.nPointsMirrorWidth+mysetting.nPointsPerBin/2);
    minDetectRange = minDetectRange*qSin(qDegreesToRadians(mysetting.elevationAngle));

    //垂直向 距离分辨率 rangeResol
    double rangeResol = resol*(mysetting.nPointsPerBin*(1-mysetting.overlapRatio));
    rangeResol = rangeResol*qSin(qDegreesToRadians(mysetting.elevationAngle));

    for(uint i=0;i<nRB_ovlp;i++)
        Height_values[i] = minDetectRange + i*rangeResol;
}
