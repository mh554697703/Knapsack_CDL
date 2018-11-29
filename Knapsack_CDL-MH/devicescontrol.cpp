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

    Motorthread = new QThread;                 //motor��������߳�
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
    //������
    qDebug() << "stop action!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    Stop_now = true;
}

void DevicesControl::checkMotor()             //ʦ��֮ǰ����ĺ��������ֿ����������õģ���ʵ��û���ϣ���ʱ����һ��
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
// ���̶����ˣ����ת��λ�ˣ�������Ҳ������
// �����������е�׼������

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
    capture_counter = 0;        // ̽�ⷽ�����������

    Stop_now = false;
    if( mysetting.detectMode == 2)
    {
        Start_Time = QDateTime::currentDateTimeUtc();
    }
    State = WaitAndWork;
    keepWorking = true;
    Generate_freqAxis();
    SaveSpec_FileHead();        //�������ļ���д���ļ�ͷ
    SaveVelo_FileHead();        //�������ļ���д���ļ�ͷ
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
            if(Stop_now)                     //���ֹͣ��ť����
            {
                Motor->motorQuit();
                LaserPulse.closePulseLaser();
                State = WaitAndStop;
            }
            else
            {
                switch (mysetting.detectMode)
                {
                case 0:                 //����̽��
                    //�ж�̽�ⷽ����
                    if(capture_counter == mysetting.angleNum)
                    {
                        Stop_now = true;
                        Motor->motorQuit();
                        LaserPulse.closePulseLaser();
                        State = WaitAndStop;
                    }
                    break;

                case 1:                 //����̽��
                    break;

                case 2:                 //��ʱ̽��
                    // �жϷ�ﵽ��������
                    dt = currentTime.msecsTo(Start_Time);
                    if(dt > qint64(mysetting.groupTime*60*1000))  // *60s*1000ms ����ﵽ����ʱ��
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
                bool status = adq.Start_Capture();                 //��ʼ�ɼ�
                int CaptureUseTime = CM_Time->restart();
                 qDebug() << "CaptureUseTime = " <<CaptureUseTime;
                 if(status == false)
                 {
                 qDebug() << "ADQ failed";
                 return;
                 }
                 CaptureTime = QDateTime::currentDateTimeUtc();     //��¼��ǰʱ�䣬����д���ļ�
                 LaserReady = false;
                 motorInPosition = false;
                 LaserPulse.checkLaser();                           //��鼤����
                 CM_Time->restart();
                 Motor->moveRelative(-mysetting.azAngleStep);        //���ת����һ������

            //����Ϊ���ټ���ʹ洢
                 CS_Time->start();
                 LOSVelocityCal(nRB_ovlp+2, nFFT_half,
                                20, mysetting.laserWaveLength,
                                freqAxis, adq.get_PSD_double());     //������ټ���

                 //****ʸ�����ٺϳ�
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
                         //�˲����Ҳ����
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
                                 losVelocityPerHeight[j]=losVelocityMat(i,j); //ÿ���߶Ȳ��������ľ������
                             }
                             vectorVelocity = fswf.getVelocity(mysetting.nDirsVectorCal,sigama2,mysetting.elevationAngle,azAngle,losVelocityPerHeight);
                             //���ݷ��صľ���������εõ�ˮƽ���١�ˮƽ���򡢴�ֱ����
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
                         //ֱ�����Ҳ����
                         std::cout << "azimuthAngle = " << azimuthAngle << std::endl;
                         DSWF dswf(mysetting.elevationAngle,azimuthAngle,mysetting.nDirsVectorCal,nRB_ovlp,losVelocityMat);
                         hVelocity = dswf.getHVelocity();
                         hAngle = dswf.getHAngle();
                         vVelocity = dswf.getVVelocity();
                     }
                     int calculateUseTime = CS_Time->restart();
                     qDebug()<<"calculateUseTime = "<<calculateUseTime;
                     SaveVelo_AddData();     // �洢ʸ�����ٵ��ļ�
                     //������ʾ
                     emit hVelocityReady(hVelocity,nRB_ovlp);
                     emit hAngleReady(hAngle,nRB_ovlp);
                     emit vVelocityReady(vVelocity,nRB_ovlp);
                 }

                 SaveSpec_AddData();         //�洢�����׵��ļ�
                 int saveUseTime = CS_Time->restart();
                 qDebug()<<"saveUseTime = "<<saveUseTime;

                 capture_counter++;
                 qDebug() << "capture_counter = " << capture_counter;
                 if(mysetting.detectMode == 1 && capture_counter == mysetting.nDirsPerFile)  //�ﵽ���ļ�������
                 {
                     SaveSpec_FileHead();
                     SaveVelo_FileHead();
                 }
               }
            }
        }
        break;

    case WaitAndStop:
        if(LaserCloseOk && MotorCloseOk)               //�ж��Ƿ񶼹�����
        {
            qDebug() << "all close ok!!!";
            if(mysetting.detectMode == 2)              //��Ϊ��ʱ̽��
            {
                State = Standby;
            }
            else
            {
                keepWorking = false;                    //�ڴ˴�����whileѭ������֮��Ҫ��ʲô�أ���
                qDebug() << "working end!!!";
            }
        }
        break;

    case Standby:
        currentTime = QDateTime::currentDateTimeUtc();
        dt = currentTime.msecsTo(Start_Time);
        if(dt > qint64(mysetting.intervalTime*60*1000))  // *60s? *1000ms?      //����ﵽ����ʱ��
        {
            Start_Time = Start_Time.addMSecs(mysetting.intervalTime*60*1000);   //���¿�ʼʱ�䣬Ϊ���´μ�ʱ
            if (mysetting.azAngleStep != 0)
            {
                capture_counter = 0;        // ̽�ⷽ�����������,�Ƿ���Ҫ��
                LaserReady = false;
                motorInPosition = false;
                Stop_now = false;

                Motor->prepare(headAngle);
                LaserSeed.beginSeedLaser(mysetting.laserLocalPower,mysetting.laserPulseEnergy);

                SaveSpec_FileHead();        //�������ļ���д���ļ�ͷ
                SaveVelo_FileHead();        //�������ļ���д���ļ�ͷ
                State = WaitAndWork;

            }
        }
        break;

    default:
        break;
    }
    mySleep(100);                   //��ʱ100����
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

// ����Ƶ�������ᣬ���ڼ��㾶����٣����ᱻд�뵽�����׵ļ�¼�ļ�
void DevicesControl::Generate_freqAxis()
{
    for (int i = 0; i < nFFT_half; i++)  {
        freqAxis[i] = mysetting.sampleFreq/2.0*(i+1)/nFFT_half;
        //        qDebug() << freqAxis[i];
    }
}

// ���㾶�����
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

    uint startIndex = aomIndex - objSpecPoints;  // Ŀ������������
    uint endIndex = aomIndex + objSpecPoints;    // Ŀ����������յ�

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

// ��ʼ���������ݻ������������ڴ�ռ�
void DevicesControl::Init_Buffers()
{
    losVelocity = new double[nRB_ovlp];
    aomSpec = new double[nFFT_half];
    specArray = new double[nRB_ovlp * nFFT_half];
    adq.Init_Buffers();
}

// ����ʸ�����ٷ��ݽ�������ļ�ͷ
void DevicesControl::SaveVelo_FileHead()
{
    Create_DataFolder();
    VeloFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").Velo";
    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ VeloFileName +").Velo";
    //    qDebug()<<"VeloFileName = "<<VeloFileName;
    QFile outputVelo(VeloFileName);

    if(outputVelo.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate��ʾ��ԭ�ļ��������
    {
        QTextStream VeloFile(&outputVelo);
        VeloFile << "Knapsack Coherent Doppler Lidar Velocity Vector" << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                                // =�ָ���
        VeloFile << endl<< "Data File Version: 1.0" << endl;          // �ļ����Ͱ汾
        VeloFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        VeloFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //�������
        VeloFile << "LaserMode:        ";
        if(/* DISABLES CODE */ (1))
        {
            VeloFile << "Pulse" << endl;		//����̽�⣨true��or����̽�⣨false�� bool
            VeloFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //������������λ��J������ģʽ��Ϊ0
        }
        else
        {
            VeloFile << "Continuous" << endl;	//����̽�⣨true��or����̽�⣨false�� bool
            VeloFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl; //���⹦�ʣ���λmW������ģʽ��Ϊ0
        }
        VeloFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;			//����Ƶ��
        VeloFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;     //������
        VeloFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;     //���Ⲩ��
        VeloFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;        //AOM��Ƶ��

        //ɨ�����
        VeloFile << "detectMode:       " ;      //̽�ⷽʽ��0����̽��1����̽��2��ʱ̽��
        switch (mysetting.detectMode) {
        case 1:                 //����̽��
            VeloFile << "NonStop" << endl;
            break;
        case 0:                 //����̽��
            VeloFile << "SingleGroup" << endl;
            break;
        case 2:                 //��ʱ̽��
            VeloFile << "scheduled" << endl;
            break;
        }

        VeloFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//������
        VeloFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//��ʼ��
        VeloFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;	    //������
        VeloFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//������
        VeloFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//��ʱ̽��������λ������
        VeloFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//��ʱ̽�ⵥ��ʱ�䣬��λ������

        //��������
        VeloFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //����Ƶ��
        VeloFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //������ƽ
        VeloFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //Ԥ�����������������ݲ��ṩ����

        //ʵʱ�������
        VeloFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //�������ۼ�������
        VeloFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //��������
        VeloFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //�������ڵ���

        VeloFile << "Height Axis:      ";
        QString str= "";
        CalHeightsValues();
        for(uint i=0;i<nRB_ovlp;i++)
            str = str + QString::number(Height_values[i],'f', 2) + " ";
        VeloFile << str << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                    // =�ָ���
        VeloFile << endl;
        outputVelo.close();
        //        qDebug() << "Velofile Header added!";
    }
}

// ���湦���׼����������ļ�ͷ
void DevicesControl::SaveSpec_FileHead()
{
    Create_DataFolder();
    SpecFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").spec";
    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
    //    qDebug()<<"SpecFileName = "<<SpecFileName;
    QFile outputSpec(SpecFileName);

    if(outputSpec.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate��ʾ��ԭ�ļ��������
    {
        QTextStream specFile(&outputSpec);
        specFile << "Knapsack Coherent Doppler Lidar Original Spectrum" << endl;
        for(int i=0;i<60;i++)
            specFile << "=";                                // =�ָ���
        specFile << endl<< "Data File Version: 1.0" << endl;          // �ļ����Ͱ汾
        specFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        specFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //�������
        specFile << "LaserMode:        ";
        if(/* DISABLES CODE */ (1))
        {
            specFile << "Pulse" << endl;		//����̽�⣨true��or����̽�⣨false�� bool
            specFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //������������λ��J������ģʽ��Ϊ0
        }
        else
        {
            specFile << "Continuous" << endl;	//����̽�⣨true��or����̽�⣨false�� bool
            specFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl;     //���⹦�ʣ���λmW������ģʽ��Ϊ0
        }
        specFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;                //����Ƶ��
        specFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;         //������
        specFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;         //���Ⲩ��
        specFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;            //AOM��Ƶ��

        //ɨ�����
        specFile << "detectMode:       " ;      //̽�ⷽʽ��0����̽��1����̽��2��ʱ̽��
        switch (mysetting.detectMode) {
        case 0:                 //����̽��
            specFile << "SingleGroup" << endl;
            break;
        case 1:                 //����̽��
            specFile << "NonStop" << endl;
            break;
        case 2:                 //��ʱ̽��
            specFile << "scheduled" << endl;
            break;
        }

        specFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//������
        specFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//��ʼ��
        specFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;     //������
        specFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//������
        specFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//��ʱ̽��������λ������
        specFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//��ʱ̽�ⵥ��ʱ�䣬��λ������

        //��������
        specFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //����Ƶ��
        specFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //������ƽ
        specFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //Ԥ�����������������ݲ��ṩ����

        //ʵʱ�������
        specFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //�������ۼ�������
        specFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //��������
        specFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //�������ڵ���

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
            specFile << "=";                    // =�ָ���
        specFile << endl;
        outputSpec.close();
        //        qDebug() << "Specfile Header added!";
    }
}

void DevicesControl::SaveSpec_AddData()
{
    QFile outputSpec(SpecFileName);
    if(outputSpec.open(QFile::WriteOnly|QIODevice::Append))       //׷��
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
    if(outputVelo.open(QFile::WriteOnly|QIODevice::Append))       //׷��
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
    if(!mypath.exists(mysetting.dataFilePath))		//����ļ��в����ڣ������ļ���
        mypath.mkpath(mysetting.dataFilePath);
}

// ��������߶Ȳ�ĸ߶�ֵ������д�뵽����ļ�
void DevicesControl::CalHeightsValues()
{
    //��ֱ�� ��С̽����� minDetectRange
    double resol = lightSpeed/mysetting.sampleFreq/1000000/2;        //��������ľ���ֱ���
    double minDetectRange = resol*(mysetting.nPointsMirrorWidth+mysetting.nPointsPerBin/2);
    minDetectRange = minDetectRange*qSin(qDegreesToRadians(mysetting.elevationAngle));

    //��ֱ�� ����ֱ��� rangeResol
    double rangeResol = resol*(mysetting.nPointsPerBin*(1-mysetting.overlapRatio));
    rangeResol = rangeResol*qSin(qDegreesToRadians(mysetting.elevationAngle));

    for(uint i=0;i<nRB_ovlp;i++)
        Height_values[i] = minDetectRange + i*rangeResol;
}
