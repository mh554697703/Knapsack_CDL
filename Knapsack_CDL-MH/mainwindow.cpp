#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QWidget>
#include <QLabel>
#include <QString>
#include <QGridLayout>
#include <QDockWidget>
#include "display/rt_display.h"

#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QLayout>
#include <QTextEdit>
#include <QSizePolicy>

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //配置文件读取，自动读取上次使用的配置文件
    m_setfile = new SettingFile();
    mysetting = m_setfile->readSettings();
    qDebug() << "Settings already readed";
    qDebug() << "Settings_PathName"<<mysetting.dataFilePath;
    checkDataFilePath();

    myfont.setFamily("Microsoft Yahei");
    myfont.setPointSize(14);
    createDockWindow();                                   //创建各显示窗口
    createActions();                                      //创建动作
    createMenus();                                        //创建菜单栏
    createToolBars();                                     //创建工具栏
    createStatusBar();                                    //创建状态栏

    //显示部分
    connect(hVelocityAction,SIGNAL(triggered(bool)),this,SLOT(hVelocityActionTriggered(bool)));
    connect(hAngleAction,SIGNAL(triggered(bool)),this,SLOT(hAngleActionTriggered(bool)));
    connect(vVelocityAngleAction,SIGNAL(triggered(bool)),this,SLOT(vVelocityAngleActionTriggered(bool)));
    connect(vVelocityAngleAction2,SIGNAL(triggered(bool)),this,SLOT(vVelocityAngleAction2Triggered(bool)));
    connect(hDataAction,&QAction::triggered,this,&MainWindow::hDataActionTriggered);

    UpdateHeightsValue();
    stGraph_HSpeed->initialShow(14, minDetectRange, rangeResol);
    devicesControl = new DevicesControl();
    connect(devicesControl, &DevicesControl::hVelocityReady, this, &MainWindow::updateHVelocityDisp);
    connect(devicesControl, &DevicesControl::hAngleReady, this, &MainWindow::updateHAngleDisp);
    connect(devicesControl, &DevicesControl::vVelocityReady, this, &MainWindow::updateVVelocityDisp);
    connect(devicesControl, &DevicesControl::WorkingComplete, this, &MainWindow::stopActionTriggered);

    connect(devicesControl, &DevicesControl::laserseedError, this,&MainWindow::LaserseedError);
    connect(devicesControl, &DevicesControl::laserpulseError, this,&MainWindow::LaserpulseError);
    connect(devicesControl, &DevicesControl::compassError, this,&MainWindow::CompassError);
    connect(devicesControl, &DevicesControl::adqError, this,&MainWindow::AdqError);
    connect(devicesControl, &DevicesControl::motorError, this,&MainWindow::MotorError);

    // 控制部分
    connect(stopAction, &QAction::triggered, this, &MainWindow::stopActionTriggered);
    connect(startAction, &QAction::triggered, this, &MainWindow::startActionTriggered);
    connect(quitAction, &QAction::triggered, this, &MainWindow::quitActionTriggered);
    connect(setAction,&QAction::triggered,this, &MainWindow::setActionTriggered);

    workThread = new QThread;
    devicesControl->moveToThread(workThread);
    connect(workThread, &QThread::finished, devicesControl, &QObject::deleteLater);
    workThread->start();
    isWorking = false;

    // 显示测试用
    for (int i = 0; i < 14; ++i) {
        H_speed[i] = 0;
        V_speed[i] = 0;
        H_direction[i] = 0;
    }
    // 构造定时器，设置超时为 1 秒
    TestTimer = new QTimer(this);
    connect(TestTimer, SIGNAL(timeout()), this, SLOT(changeData()));
//    TestTimer->start(1000);
    connect(compassbutton,&QPushButton::clicked ,this,&MainWindow::on_CompassButton_clicked);
}

MainWindow::~MainWindow()
{
    if(TestTimer->isActive())
    TestTimer->stop();
    delete TestTimer;
    delete ui;
    delete m_setfile;
    devicesControl->deleteLater();
}

void MainWindow::setActionTriggered()
{
    ParameterSetDialog *parameterSetDialog = new ParameterSetDialog(this);
    parameterSetDialog->setParaSettings(mysetting, isWorking);
    if (parameterSetDialog->exec() == QDialog::Accepted) {
        mysetting = parameterSetDialog->getParaSettings();
        UpdateHeightsValue();       //刷新显示
        UpdateHeightsValue();       //刷新显示
        stGraph_HSpeed->initialShow(mysetting.nRangeBin, minDetectRange, rangeResol);
    }
    delete parameterSetDialog;
}

void MainWindow::quitActionTriggered()
{
    //待完善
    this->close();
}

void MainWindow::startActionTriggered()
{  
    qDebug()<<"dock1_Hight = "<<dock1->height();
    if (!isWorking) {
        compassbutton->setStyleSheet("background-color:rgb(0,255,100);");
        seedbutton->setStyleSheet("background-color:rgb(0,255,100);");
        pulsebutton->setStyleSheet("background-color:rgb(0,255,100);");
        motorbutton->setStyleSheet("background-color:rgb(0,255,100);");
        adqbutton->setStyleSheet("background-color:rgb(0,255,100);");
        isWorking = true;
        TestTimer->start(1000);
        statusBarText->setText(QString::fromLocal8Bit("探测进行中..."));
        devicesControl->startAction(mysetting);
        toolbar->set_to_started();
    }
    else {
        stopActionTriggered();
    }
}

void MainWindow::stopActionTriggered()
{
    TestTimer->stop();
    toolbar->set_to_stopped();
    if (isWorking) {
        isWorking = false;
        devicesControl->stopAction();
    }
    statusBarText->setText(QString::fromLocal8Bit("未进行探测"));
}

void MainWindow::UpdateHeightsValue()
{
    //垂直向 最小探测距离
    double resol = lightSpeed/mysetting.sampleFreq/1000000/2;        //单采样点的径向分辨率
    minDetectRange = resol*(mysetting.nPointsMirrorWidth+mysetting.nPointsPerBin/2);
    qDebug()<<"minDetectRange ="<<minDetectRange;
    minDetectRange = minDetectRange*qSin(qDegreesToRadians(mysetting.elevationAngle));
    //垂直向 距离分辨率
    rangeResol = resol*(mysetting.nPointsPerBin*(1-mysetting.overlapRatio));
    rangeResol = rangeResol*qSin(qDegreesToRadians(mysetting.elevationAngle));
    //重叠后距离门数（0或0.5）
    uint nRB_ovlp;
    if( mysetting.overlapRatio > 0)
        nRB_ovlp = mysetting.nRangeBin*2 - 1;
    else
        nRB_ovlp = mysetting.nRangeBin;

    for(uint i=0;i<nRB_ovlp;i++)
    {
        Height_values[i] = minDetectRange + i*rangeResol;
        qDebug()<<Height_values[i];
    }
    hSpeedChart->setHeight(Height_values,nRB_ovlp);
    hAngleChart->setHeight(Height_values,nRB_ovlp);
    vSpeedBarChart->setAxis(Height_values,nRB_ovlp);
    vSpeedLineChart->setHeight(Height_values,nRB_ovlp);
}

void MainWindow::updateHVelocityDisp(double *hVelocity, uint n)
{
    hSpeedChart->updateData(hVelocity,n);
    qDebug() << "HSpeed update show";
    stGraph_HSpeed->updateShow(hVelocity);
}

void MainWindow::updateHAngleDisp(double *hAngle, uint n)
{
    hAngleChart->updateData(hAngle,n);
    qDebug() << "HAngle update show";
}

void MainWindow::updateVVelocityDisp(double *vVelocity, uint n)
{
    vSpeedBarChart->updateData(vVelocity,n);
    vSpeedLineChart->updateData(vVelocity,n);
    qDebug() << "VSpeed update show";
}

void MainWindow::checkDataFilePath()
{
    QString str = mysetting.dataFilePath;

    QDir mypath(str);
    QString dirname = mypath.dirName();
    QDateTime time = QDateTime::currentDateTime();

    int num = dirname.toInt();
    int len = dirname.length();
    if(mysetting.autoCreateDateDir)
    {
        QString today_str = time.toString("yyyyMMdd");
        int today_int = today_str.toInt();
        if(len == 8 && (num != today_int) && qAbs(num - today_int)<10000)
        {
            str = mypath.absolutePath();
            int str_len = str.length();
            str.resize(str_len - 8);
            str += today_str;
        }
        else if( dirname != time.toString("yyyyMMdd"))
        {
            str = mypath.absolutePath();
            str += QString("/");
            str += time.toString("yyyyMMdd");			//设置显示格式
            qDebug()<<"Dir not Match";
        }
        qDebug()<<str<<endl;
    }
    else												//选择不生成日期路径时，如果当前日期路径存在，则删除。
    {
        if( dirname == time.toString("yyyyMMdd"))
        {
            if (!mypath.exists())
            {
                str = mypath.absolutePath();
                int str_len = str.length();
                str.resize(str_len - 9);				//减去/20xxxxxx
            }
            qDebug()<<"Dir Match"<<str<<endl;
        }
    }
    mysetting.dataFilePath = str;
}

void MainWindow::changeData()       //测试用
{
    for (int i = 0; i < 14; ++i)
    {
        H_speed[i] += (1.3 - H_speed[i]*0.01*i);
        if(H_speed[i]>15)
            H_speed[i] -= 14.3;

        H_direction[i] += 1.3 + H_direction[i]*0.2 + i/18;
        if(H_direction[i] >= 360)
            H_direction[i] -= 360;

        V_speed[i] += 0.7-0.2*i;
        if(V_speed[i]>6)
            V_speed[i] -= 12 + i/12;
        if(V_speed[i]<-6)
            V_speed[i] += 8 + i/7;
    }
    hSpeedChart->updateData(H_speed,14);
    hAngleChart->updateData(H_direction,14);
    vSpeedBarChart->updateData(V_speed,14);
    vSpeedLineChart->updateData(V_speed,14);
    stGraph_HSpeed->updateShow(H_speed);
//    statusBarText->setText(QString::fromLocal8Bit(" 模拟测试中..."));
//    stGraph_VSpeed->updateShow(V_speed);
}

void MainWindow::createStatusBar()              //状态栏
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    QLabel *label = new QLabel;
    label->setText(QString::fromLocal8Bit(" 系统状态: "));
    label->setStyleSheet("background-color:rgb(0,250,250);");
    label->setFont(myfont);
    label->setFixedHeight(30);
    hlayout->addWidget(label);

    compassbutton = new QPushButton();
    compassbutton->setObjectName("CompassButton");
//    compassbutton->setStyleSheet("background-color:rgb(0,255,100);");
    compassbutton->setText(QString::fromLocal8Bit("罗盘"));
    compassbutton->setFont(myfont);
    compassbutton->setFixedSize(70,30);
    hlayout->addWidget(compassbutton);

    adqbutton = new QPushButton(this);
//    adqbutton->setStyleSheet("background-color:rgb(0,255,100);");
    adqbutton->setText(QString::fromLocal8Bit("采集卡"));
    adqbutton->setFont(myfont);
    adqbutton->setFixedSize(70,30);
    hlayout->addWidget(adqbutton);

    motorbutton = new QPushButton(this);
//    motorbutton->setStyleSheet("background-color:rgb(0,255,100);");
    motorbutton->setText(QString::fromLocal8Bit("电机"));
    motorbutton->setFont(myfont);
    motorbutton->setFixedSize(70,30);
    hlayout->addWidget(motorbutton);

    seedbutton = new QPushButton(this);
//    seedbutton->setStyleSheet("background-color:rgb(0,255,100);");
    seedbutton->setText(QString::fromLocal8Bit("种子源"));
    seedbutton->setFont(myfont);
    seedbutton->setFixedSize(70,30);
    hlayout->addWidget(seedbutton);

    pulsebutton = new QPushButton(this);
//    pulsebutton->setStyleSheet("background-color:rgb(0,255,100);");
    pulsebutton->setText(QString::fromLocal8Bit("放大器"));
    pulsebutton->setFont(myfont);
    pulsebutton->setFixedSize(70,30);
    hlayout->addWidget(pulsebutton);
    hlayout->setSpacing(20);

    QWidget *StatusWidget = new QWidget;
    StatusWidget->setLayout(hlayout);

    statusBarText = new QLabel(QString::fromLocal8Bit(" 未进行探测"));
    statusBarText->setFont(myfont);
    statusBarText->setStyleSheet("background-color:rgb(0,255,100);");
    statusBarText->setFixedSize(350,30);

    ui->statusBar->setFixedHeight(45);
    ui->statusBar->addWidget(StatusWidget);
    ui->statusBar->setStyleSheet(QString("QStatusBar::item{border: 0px}"));    //隐藏状态栏边框
    QLabel *label2 = new QLabel;
    label2->setFont(myfont);
    label2->setFixedHeight(30);
    label2->setText(QString::fromLocal8Bit(" 工作状态: "));
    label2->setStyleSheet("background-color:rgb(0,250,250);");
    ui->statusBar->addPermanentWidget(label2);
    ui->statusBar->addPermanentWidget(statusBarText);


}

void MainWindow::createDockWindow()           //各个显示窗口
{
    hSpeedChart = new LineChart;                                 //水平风速
    hSpeedChart->legend()->hide();
    hSpeedChart->setMinimumWidth(250);
    QChartView *hSpeedChartView = new QChartView(hSpeedChart);
    hSpeedChartView->setRenderHint (QPainter::Antialiasing);       //对图像进行抗锯齿处理

    hAngleChart = new polarchart;                             //水平风向
    hAngleChart->legend()->hide();
    hAngleChart->setMinimumSize(250,600);
    QChartView *hAngleChartView = new QChartView(hAngleChart);

    vSpeedBarChart = new barchart;                              //垂直风柱形图
    vSpeedBarChart->legend()->hide();
    vSpeedBarChart->setMinimumSize(250,600);
    vSpeedBarChart->setAutoFillBackground(true);
    vSpeedChartView = new QChartView(vSpeedBarChart);

    vSpeedLineChart = new LineChart;        //垂直风折线图
    vSpeedLineChart->legend()->hide();
    vSpeedLineChart->setMinimumSize(250,600);
    vSpeedLineChart->setAutoFillBackground(true);
    vSpeedChart2View = new QChartView(vSpeedLineChart);

    stGraph_HSpeed = new STGraph();                           //水平风场时空图
    stGraph_HSpeed->setMinimumSize(250,600);

    //水平风速时空图窗口
    dock1 = new QDockWidget(tr("DockWindow 1"),this);
    dock1->setWindowTitle(QString::fromLocal8Bit("水平风速时空分布"));
    dock1->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock1->setAllowedAreas(Qt::TopDockWidgetArea|Qt::LeftDockWidgetArea);
    dock1->setWidget(stGraph_HSpeed);
    addDockWidget(Qt::TopDockWidgetArea,dock1);

    //水平风速/风向窗口
    dock2 = new QDockWidget(QString::fromLocal8Bit("水平风速(m/s)"),this);
    dock2->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock2->setWidget(hSpeedChartView);
    addDockWidget(Qt::TopDockWidgetArea,dock2);

    dock3 = new QDockWidget(QString::fromLocal8Bit("水平风向(m/s)"),this);
    dock3->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock3->setWidget(hAngleChartView);
    addDockWidget(Qt::TopDockWidgetArea,dock3);

    //垂直风速窗口
    dock4 = new QDockWidget(QString::fromLocal8Bit("垂直风速(m/s)与风向"),this);
    dock4->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock4->setAllowedAreas(Qt::TopDockWidgetArea|Qt::RightDockWidgetArea);
    dock4->setWidget(vSpeedChartView);
    dock4->setVisible(false);
    addDockWidget(Qt::TopDockWidgetArea,dock4);

    dock5 = new QDockWidget(QString::fromLocal8Bit("垂直风速(m/s)与风向"),this);
    dock5->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock5->setAllowedAreas(Qt::TopDockWidgetArea|Qt::RightDockWidgetArea);
    dock5->setWidget(vSpeedChart2View);
    addDockWidget(Qt::TopDockWidgetArea,dock5);
    qDebug()<<"createDockWindow";
}

void MainWindow::createMenus()              //菜单栏
{
    startMenu = menuBar()->addMenu(QString::fromLocal8Bit("探测"));
    startMenu->setFont(myfont);
    startMenu->addAction(startAction);
    startMenu->addAction(stopAction);
    setMenu = menuBar()->addMenu(QString::fromLocal8Bit("设置"));
    setMenu->addAction(setAction);
    quitMenu = menuBar()->addMenu(QString::fromLocal8Bit("退出"));
    quitMenu->addAction(quitAction);
    showMenu = menuBar()->addMenu(QString::fromLocal8Bit("显示"));
    showMenu->setFont(myfont);
    showMenu->addAction(hDataAction);
    showMenu->addAction(hVelocityAction);
    showMenu->addAction(hAngleAction);
    vVelocityAngleMenu = showMenu->addMenu(QString::fromLocal8Bit("垂直风速/风向"));

    vVelocityAngleMenu->setFont(myfont);
    vVelocityAngleMenu->addAction(vVelocityAngleAction2);
    vVelocityAngleMenu->addAction(vVelocityAngleAction);
    qDebug()<<"createMenus";
}

void MainWindow::createToolBars()           //工具栏
{
    qDebug()<<"createToolBar";
    toolbar = new UserToolBar();
    addToolBar(Qt::TopToolBarArea,toolbar);
    connect(toolbar->quitAction, &QAction::triggered, this, &MainWindow::quitActionTriggered);
    connect(toolbar->setAction,&QAction::triggered,this, &MainWindow::setActionTriggered);
    connect(toolbar->startAction,&QAction::triggered,this, &MainWindow::startActionTriggered);
//    connect(toolbar->stopAction,&QAction::triggered,this, &MainWindow::stopActionTriggered);
    toolbar->setVisible(true);
}
void MainWindow::createActions()          //菜单栏动作
{
    setAction =new QAction(QString::fromLocal8Bit("设置"));
    startAction = new QAction(QString::fromLocal8Bit("开始探测"));
    stopAction = new QAction(QString::fromLocal8Bit("停止探测"));
    quitAction = new QAction(QString::fromLocal8Bit("退出"));
    hVelocityAction = new QAction(QString::fromLocal8Bit("水平风速"));
    hAngleAction = new QAction(QString::fromLocal8Bit("水平风向"));
    vVelocityAngleAction = new QAction(QString::fromLocal8Bit("柱形图"));
    hDataAction = new QAction(QString::fromLocal8Bit("水平风速时空分布"));
    vVelocityAngleAction2 = new QAction(QString::fromLocal8Bit("折线图"));

    startAction->setFont(myfont);
    setAction->setFont(myfont);
    stopAction->setFont(myfont);
    quitAction->setFont(myfont);
    hVelocityAction->setFont(myfont);
    hAngleAction->setFont(myfont);
    vVelocityAngleAction->setFont(myfont);
    vVelocityAngleAction2->setFont(myfont);
    hDataAction->setFont(myfont);

    hVelocityAction->setCheckable(true);
    hVelocityAction->setChecked(true);
    hAngleAction->setCheckable(true);
    hAngleAction->setChecked(true);
    vVelocityAngleAction->setCheckable(true);
    vVelocityAngleAction->setChecked(false);
    vVelocityAngleAction2->setCheckable(true);
    vVelocityAngleAction2->setChecked(true);
    hDataAction->setCheckable(true);
    hDataAction->setChecked(true);
    qDebug()<<"createActions";
}

void MainWindow::hVelocityActionTriggered(const bool a)
{
    dock2->setVisible(a);
}

void MainWindow::hAngleActionTriggered(const bool a)
{
    dock3->setVisible(a);
}
void MainWindow::vVelocityAngleActionTriggered(const bool a)
{
    vVelocityAngleAction->setChecked(a);
    dock4->setVisible(a);
    if(dock5->isVisible())
    {
        vVelocityAngleAction2->setChecked(false);
        dock5->setVisible(false);
    }
}

void MainWindow::vVelocityAngleAction2Triggered(const bool a)
{
    vVelocityAngleAction2->setChecked(a);
    dock5->setVisible(a);
    if(dock4->isVisible())
    {
        vVelocityAngleAction->setChecked(false);
        dock4->setVisible(false);
    }
}

void MainWindow::hDataActionTriggered(const bool a)
{
    dock1->setVisible(a);
}

void MainWindow::MotorError(const QString &s)
{
    motorbutton->setStyleSheet("background-color:rgb(255,0,100);");
    statusBarText->setText(s);
}

void MainWindow::CompassError(const QString &s)
{
    compassbutton->setStyleSheet("background-color:rgb(255,0,100);");
    statusBarText->setText(s);
}

void MainWindow::LaserseedError(const QString &s)
{
    seedbutton->setStyleSheet("background-color:rgb(255,0,100);");
    statusBarText->setText(s);
}

void MainWindow::LaserpulseError(const QString &s)
{
    pulsebutton->setStyleSheet("background-color:rgb(255,0,100);");
    statusBarText->setText(s);
}

void MainWindow::AdqError(const QString &s)
{
    adqbutton->setStyleSheet("background-color:rgb(255,0,100);");
    qDebug()<<"adqError = "<<s;
    statusBarText->setText(s);
}

void MainWindow::on_CompassButton_clicked()            //测试用
{
    qDebug()<<"window width ="<<this->width();
    qDebug()<<"window heigh ="<<this->height();
    qDebug()<<"button heigh ="<<compassbutton->height();
}

void MainWindow::changeEvent(QEvent *event)           //窗口最大化时重新调整图表大小
{
    if(event->type()!=QEvent::WindowStateChange) return;
       if(this->windowState()==Qt::WindowMaximized)
       {
           dock3->setMinimumHeight(height()-150);
       }
}

void MainWindow::resizeEvent(QResizeEvent * event)    //窗口尺寸变换时重新调整图表大小
{
//    QMainWindow::resizeEvent(event);
//    hSpeedChart->setMinimumHeight(height()-168);
    dock3->setMinimumHeight(height()-150);
//    emit size_changed();
}
