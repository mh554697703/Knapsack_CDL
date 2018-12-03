#include "ADQ214.h"
#include <QMessageBox>
#include <QThread>

const unsigned int adq_num = 1;

ADQ214::ADQ214(QObject *parent) : QObject(parent)
{
    adq_cu = CreateADQControlUnit();
    success = true;

    int apirev = ADQAPI_GetRevision();     // 获取API版本
    qDebug()<<"API_Revision = "<<apirev;
    connectADQDevice();

    num_of_devices = 0;
    num_of_failed = 0;
    num_of_ADQ214 = 0;

    setupadq.stream_ch = ADQ214_STREAM_ENABLED_BOTH;
    setupadq.stream_ch &= 0x7;
    setupadq.num_buffers = 64;
    setupadq.size_buffers = 1024;
    setupadq.clock_source = 0;       //0 = Internal clock
    setupadq.pll_divider = 2;        //在Internal clock=0时，设置，f_clk = 800MHz/divider
    psd_res = nullptr;
    psd_array = nullptr;
//    captureTimer = new QTimer;
//    connect(captureTimer, SIGNAL(timeout()), this, SLOT(capturetimeout));
}

void ADQ214::Transfer_Settings(const SOFTWARESETTINGS &settings)
{
    mainSettings = settings;
    qDebug() << "nPSD" << nPSD;
    if (mainSettings.overlapRatio == 0.5)
        nPSD = (mainSettings.nRangeBin*2 + 1) * nFFT_half;  // PSD功率谱长度
    else
        nPSD = (mainSettings.nRangeBin+2) * nFFT_half;      // PSD功率谱长度
}

PSD_DATA* ADQ214::get_PSD_Union()
{
    return psd_res;
}

double* ADQ214::get_PSD_double()
{
    return psd_array;
}

void ADQ214::connectADQDevice()
{
    int num_of_devices,num_of_failed,num_of_ADQ214;
    num_of_devices = ADQControlUnit_FindDevices(adq_cu);			//找到所有与电脑连接的ADQ，并创建一个指针列表，返回找到设备的总数
    num_of_failed = ADQControlUnit_GetFailedDeviceCount(adq_cu);
    num_of_ADQ214 = ADQControlUnit_NofADQ214(adq_cu);				//返回找到ADQ214设备的数量
    if((num_of_failed > 0)||(num_of_devices == 0))
    {

        isADQ214Connected = false;
        QString errorcode  = QString::fromLocal8Bit("采集卡未连接");
        emit adqError(errorcode);
        qDebug()<<QString::fromLocal8Bit("采集卡未连接");
    }
    else if (num_of_ADQ214 != 0) {
        qDebug()<<QString::fromLocal8Bit("采集卡已连接");
        isADQ214Connected = true;
    }
}

bool ADQ214::Start_Capture()
{
    if(!Config_ADQ214())
        return false;
    setupadq.data_stream_target = new quint16[nPSD * 4];
    memset(setupadq.data_stream_target, 0, nPSD * 8);

    if(!CaptureData2Buffer()) {
        qDebug() << ("Collect failed!");
        QString errorcode  = "Collect failed!";
        emit adqError(errorcode);
        delete setupadq.data_stream_target;
        return false;
    }
    qDebug() << "Collect finished!";
    ConvertData2Spec();
//    WriteSpecData2disk();
    delete setupadq.data_stream_target;

    if(success == 0) {
        qDebug() << "Error!";
        emit adqError("Error!");
        DeleteADQControlUnit(adq_cu);
    }
    qDebug() << "Write finished";
    return true;
}

bool ADQ214::Config_ADQ214()                   // 配置采集卡
{
    success = false;
    if (!isADQ214Connected) {
        //        QMessageBox::critical(NULL, QString::fromStdString("采集卡未连接！！"), QString::fromStdString("采集卡未连接"),
        //                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        qDebug()<< "ADQ214 Connect failed!!!!";
        emit adqError("ADQ214 Connect failed!!!!");
    }
    else {
        success = ADQ214_SetDataFormat(adq_cu, adq_num,ADQ214_DATA_FORMAT_UNPACKED_16BIT);
        int num_buffers = 256;
        int size_buffers = 1024;
        success = success && ADQ214_SetTransferBuffers(adq_cu, adq_num, num_buffers, size_buffers);  // 设置TransferBuffer大小及数量
        success = success && ADQ214_SetClockSource(adq_cu, adq_num, setupadq.clock_source);          // 设置时钟源
        success = success && ADQ214_SetPllFreqDivider(adq_cu, adq_num, setupadq.pll_divider);        // 设置PLL分频数

        // 配置ADQ214的内部寄存器

        // quint16 CMD = 0;
        // success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,CMD);                      // 命令
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x31,0,mainSettings.triggerLevel);   // 触发电平
        //        qDebug()<<"0x31,0,TriggerLevel  " << mainSettings.triggerLevel;
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x32,0,mainSettings.nPulsesAcc);     // 累加脉冲数
        //        qDebug()<<"0x32,0,nPulsesAcc    " << mainSettings.nPulsesAcc;
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x33,0,mainSettings.nPointsPerBin);  // 距离门点数，需要为偶数
        //        qDebug()<<"0x33,0,nPointsPerBin    " << mainSettings.nPointsPerBin;
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x34,0,mainSettings.nRangeBin+3);    // 距离门数
        //        qDebug()<<"0x34,0,nRangeBin        " << mainSettings.nRangeBin+3;
        if (mainSettings.overlapRatio == 0.5)
        {
            int Overlap_length = (mainSettings.nRangeBin -0.5)*mainSettings.nPointsPerBin;
            success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x35,0,Overlap_length);          // OverLap计数器长度
            //            qDebug()<<"0x35,0,Overlap_length    " << Overlap_length;
        }
        else
        {
            success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x35,0,0);                       // OverLap计数器长度
            //            qDebug()<<"0x35,0,Overlap_length    " << 0;
        }
        int Mirror_Start = 500 + mainSettings.nPointsMirrorWidth - mainSettings.nPointsPerBin;
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x36,0,Mirror_Start);                // 镜面所在距离门起始位置点
        //        qDebug()<<"0x36,0,Mirror_Start    " << Mirror_Start;
        int Total_Points_Num = mainSettings.nPointsPerBin * mainSettings.nRangeBin + 500 + mainSettings.nPointsMirrorWidth;     // 内部信号，处理总点数
        success = success && ADQ214_WriteAlgoRegister(adq_cu,1,0x37,0,Total_Points_Num);            // 单脉冲处理结束位置点
        //        qDebug()<<"0x37,0,Total_Points_Num    " << Total_Points_Num;
    }
    return success;
}

bool ADQ214::CaptureData2Buffer()         // 采集数据到缓存
{
    qDebug() << "start capture to buffer";
//    captureTimer->start(5000);

    success = ADQ214_DisarmTrigger(adq_cu, adq_num);
    success = success && ADQ214_SetStreamStatus(adq_cu, adq_num,setupadq.stream_ch);
    success = success && ADQ214_ArmTrigger(adq_cu, adq_num);

    unsigned int samples_to_collect;
    samples_to_collect = nPSD * 4;
    unsigned int total_sample_points = samples_to_collect;

    int nloops = 0;

    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0xFFFE);   // bit[0]置0
    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0x0001);   // bit[0]置1
    if (setupadq.trig_mode == 1) {
        ADQ214_SWTrig(adq_cu, adq_num);
    }

    while (samples_to_collect > 0)
    {
        nloops ++;
        ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0xFFFE);   // bit[0]置0
        ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,0x0001);   // bit[0]置1

        int lop = 0;
        do {
            ++lop;
            setupadq.collect_result = ADQ214_GetTransferBufferStatus(adq_cu, adq_num, &setupadq.buffers_filled);
            QThread::msleep(1);
        } while ((setupadq.buffers_filled == 0) && (setupadq.collect_result) && lop < 30000);

        setupadq.collect_result = ADQ214_CollectDataNextPage(adq_cu, adq_num);

        int samples_in_buffer = qMin(ADQ214_GetSamplesPerPage(adq_cu, adq_num), samples_to_collect);

        if (ADQ214_GetStreamOverflow(adq_cu, adq_num)) {
            setupadq.collect_result = 0;
        }

        if (setupadq.collect_result) {
            memcpy((void*)&setupadq.data_stream_target[total_sample_points - samples_to_collect],
                    ADQ214_GetPtrStream(adq_cu, adq_num), samples_in_buffer* sizeof(quint16));
            samples_to_collect -= samples_in_buffer;
        }
        else {
            qDebug() << ("Collect next data page failed!");
            samples_to_collect = 0;
        }
    }

    success = success && ADQ_DisarmTrigger(adq_cu, adq_num);

    success = success && ADQ214_SetStreamStatus(adq_cu, adq_num,0);
    return success;
}

void ADQ214::WriteSpecData2disk()         // 将数据转换成功率谱，写入到文件
{
    QFile Specfile("data_Spec.txt");
    if(Specfile.open(QFile::WriteOnly)) {
        QTextStream out(&Specfile);
        for (uint k=0; (k<(mainSettings.nRangeBin+2)*nFFT_half); k++) {
            out <<psd_res[k].data64 << endl;
        }
        Specfile.close();
    }
}

void ADQ214::ConvertData2Spec()           // 将数据转换成功率谱
{
    qDebug() << "Start Convert!!";

    int i = 0, k = 0, l = 0;
    int TotalRangeBin =nPSD/512;
    for (l=0;l<TotalRangeBin;l++) {
        for (k=0,i=0; (k<nFFT_half); k++,k++) {
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[3] = setupadq.data_stream_target[nFFT*2*l + i];
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[2] = setupadq.data_stream_target[nFFT*2*l + i+1];
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[1] = setupadq.data_stream_target[nFFT*2*l + i+4];
            psd_res[nFFT_half*l + nFFT_half-1 - k].pos[0] = setupadq.data_stream_target[nFFT*2*l + i+5];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[3] = setupadq.data_stream_target[nFFT*2*l + i+2];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[2] = setupadq.data_stream_target[nFFT*2*l + i+3];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[1] = setupadq.data_stream_target[nFFT*2*l + i+6];
            psd_res[nFFT_half*l + nFFT_half-1 - k-1].pos[0] = setupadq.data_stream_target[nFFT*2*l + i+7];

            i = i + 8;
        }
        //        qDebug() << "l" << l;
    }

    for (k=0; k<nPSD; k++) {
        psd_array[k] = double(psd_res[k].data64);
    }
    qDebug() << "Covert finished!!!";
}

void ADQ214::Init_Buffers()           // 初始化功率谱数据存储缓冲区
{
    psd_array = new double[nPSD];      //待优化，不用每次new
    psd_res = new PSD_DATA[nPSD];
}

void ADQ214::capturetimeout()
{
    captureTimer->stop();
    if(!success)
    {
        QString errorcode = QString::fromLocal8Bit("采集超时");
        emit adqError(errorcode);
    }
}
