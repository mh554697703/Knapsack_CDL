#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H
#include <QString>


// 与采集卡有关的，通过API访问的参数
typedef struct
{
    int apirev;
    int stream_ch;
    int clock_source;                   //时钟源
    int pll_divider;
    int trig_mode;

    unsigned int buffers_filled;
    unsigned int collect_result;
    int num_buffers;			         //Number of buffers
    int size_buffers;			         //Size of each buffer
    quint16* data_stream_target;
}setupADQ;

typedef struct
{
    //Laser Parameters
    double laserPulseEnergy;    //激光能量，单位μJ
    double laserLocalPower;     //激光功率，单位mW
    double laserRPF;            //激光频率
    double laserPulseWidth;     //脉冲宽度，单位ns
    double laserWaveLength;     //激光波长，单位μm
    double laserAOMFreq;        //AOM移频量，单位MHz

    //Scan Parameters
    uint    detectMode;         //探测方式：0单组探测1持续探测2定时探测
    double  elevationAngle;     //俯仰角，单位°
    double  azAngleStart;       //起始角，单位°
    double  azAngleStep;        //步进角，单位°
    uint    angleNum;           //方向数
    double  circleNum;          //圆周数
    bool    isAngleChecked;     //方向键
    bool    isCircleChecked;    //圆周键
    double  motorSpeed;         //驱动器速度
    double  intervalTime;       //定时探测间隔，单位：分钟
    double  groupTime;          //定时探测单组时间，单位：分钟

    //Sample Parameters
    double  sampleFreq;         //采样频率，单位MHz
    uint    triggerLevel;       //触发电平
    uint    nPointsPreTrigger;  //预触发点数，保留，暂不提供设置

    //Real Time Process
    uint    nPulsesAcc;         //单方向累加脉冲数
    uint    nRangeBin;          //距离门数，不包括镜面和噪声距离门（及其间隔）
    uint    nPointsPerBin;      //距离门内点数
    uint    nPointsMirrorWidth; //镜面宽度点数
    double  overlapRatio;       //距离门重叠率，0:0 1:0.5

    //Vector Velocity Inversion
    uint    nPointsObjFreq;     //径向风速范围，存储为点数，显示为速度
    uint    nDirsVectorCal;     //计算矢量风速所需的径向风速数量

    //File Store
    QString dataFilePath;       //数据存储路径
    bool    autoCreateDateDir;  //自动创建日期文件夹
    uint    nDirsPerFile;       //持续模式下，单文件内的方向数
}SOFTWARESETTINGS;

union PSD_DATA
{
    quint64 data64;
    ushort  pos[4];
};

// 经过BitReverseIndex索引后的数据，[0:511]为1024点FFT功率谱的[1024:-1:513]
// 对称于Matlab功率谱的[2:513]，无零频
static const unsigned short BitReverseIndex[] =
{
    511,255,383,127,447,191,319,63,479,223,351,95,415,159,287,31,
    495,239,367,111,431,175,303,47,463,207,335,79,399,143,271,15,
    503,247,375,119,439,183,311,55,471,215,343,87,407,151,279,23,
    487,231,359,103,423,167,295,39,455,199,327,71,391,135,263,7 ,
    507,251,379,123,443,187,315,59,475,219,347,91,411,155,283,27,
    491,235,363,107,427,171,299,43,459,203,331,75,395,139,267,11,
    499,243,371,115,435,179,307,51,467,211,339,83,403,147,275,19,
    483,227,355,99,419,163,291,35,451,195,323,67,387,131,259,3  ,
    509,253,381,125,445,189,317,61,477,221,349,93,413,157,285,29,
    493,237,365,109,429,173,301,45,461,205,333,77,397,141,269,13,
    501,245,373,117,437,181,309,53,469,213,341,85,405,149,277,21,
    485,229,357,101,421,165,293,37,453,197,325,69,389,133,261,5 ,
    505,249,377,121,441,185,313,57,473,217,345,89,409,153,281,25,
    489,233,361,105,425,169,297,41,457,201,329,73,393,137,265,9 ,
    497,241,369,113,433,177,305,49,465,209,337,81,401,145,273,17,
    481,225,353,97,417,161,289,33,449,193,321,65,385,129,257,1  ,
    510,254,382,126,446,190,318,62,478,222,350,94,414,158,286,30,
    494,238,366,110,430,174,302,46,462,206,334,78,398,142,270,14,
    502,246,374,118,438,182,310,54,470,214,342,86,406,150,278,22,
    486,230,358,102,422,166,294,38,454,198,326,70,390,134,262,6 ,
    506,250,378,122,442,186,314,58,474,218,346,90,410,154,282,26,
    490,234,362,106,426,170,298,42,458,202,330,74,394,138,266,10,
    498,242,370,114,434,178,306,50,466,210,338,82,402,146,274,18,
    482,226,354,98,418,162,290,34,450,194,322,66,386,130,258,2  ,
    508,252,380,124,444,188,316,60,476,220,348,92,412,156,284,28,
    492,236,364,108,428,172,300,44,460,204,332,76,396,140,268,12,
    500,244,372,116,436,180,308,52,468,212,340,84,404,148,276,20,
    484,228,356,100,420,164,292,36,452,196,324,68,388,132,260,4 ,
    504,248,376,120,440,184,312,56,472,216,344,88,408,152,280,24,
    488,232,360,104,424,168,296,40,456,200,328,72,392,136,264,8 ,
    496,240,368,112,432,176,304,48,464,208,336,80,400,144,272,16,
    480,224,352,96,416,160,288,32,448,192,320,64,384,128,256,0
};

const int Max_nLayers = 30;         //最大高度层数 = 30
const int nFFT = 1024;              //FFT点数数 = 1024
const int nFFT_half = nFFT/2;       //FFT点数数的一半 = 512
const int stateCheckPeriod = 40;    //采集过程中，状态转换定时器的查询间隔（ms）
const int motorCheckPeriod = 100;   //采集过程中，电机MS判断定时器的查询间隔（ms）
const double lightSpeed = 299792458;
enum Control_State{ WaitMotor, Capture, Standby ,Stop, Quit, WaitAndWork,WaitAndStop };     //探测过程控制状态值
const QString SeedLaserComPort  = "COM1";   // 种子激光器串口号
const QString PulseLaserComPort = "COM2";   // 脉冲放大器串口号
const QString CompassComPort    = "COM4";   // 电子罗盘串口号
const QString MotorComPort      = "COM8";   // 伺服电机控制器串口号
#endif // GLOBAL_DEFINES_H
