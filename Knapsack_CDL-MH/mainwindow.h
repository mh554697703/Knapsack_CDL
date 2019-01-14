#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "global_defines.h"
#include "settingfile.h"
#include "parametersetdialog.h"
#include <QFont>
#include <QPushButton>
#include <QDockWidget>
#include <QMenuBar>
#include "display/lineChart.h"
#include "display/polarchart.h"
#include "display/barchart.h"

#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QChartView>
#include "usertoolbar.h"
#include "admintoolbar.h"
#include "devicescontrol.h"
#include "display/wind_display.h"
#include "display/stgraph.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createMenus();
    void createToolBars();
    void createActions();
    void createStatusBar();
    void createDockWindow();
    int H_low;
    int H_high;

    double H_speed[Max_nLayers];
    double H_direction[Max_nLayers];
    double V_speed[Max_nLayers];
    double Height_values[Max_nLayers];

public slots:
    void changeData();
signals:
    void size_changed();                    // 用于调整实时风速显示的widget高度
    void TestSignal(const QString s);

private slots:
    void setActionTriggered();			//设置
    void startActionTriggered();          //启动
    void stopActionTriggered();          //停止
    void quitActionTriggered();           //退出
    void hVelocityActionTriggered(const bool a);
    void hAngleActionTriggered(const bool a);
    void vVelocityAngleActionTriggered(const bool a);
    void hDataAction1Triggered(const bool a);
    void hDataAction2Triggered(const bool a);
    void vVelocityAngleAction2Triggered(const bool a);
    // GUI界面相关
    void UpdateHeightsValue();

    void updateHVelocityDisp(double *hVelocity, uint n);
    void updateHAngleDisp(double *hAngle, uint n);
    void updateVVelocityDisp(double *vVelocity, uint n);

    void checkDataFilePath();

    void MotorError(const QString &s);
    void CompassError(const QString &s);
    void LaserseedError(const QString &s);
    void LaserpulseError(const QString &s);
    void AdqError(const QString &s);
//    void on_CompassButton_clicked();
    void changeEvent(QEvent *event);

private:
    Ui::MainWindow *ui;

    bool isWorking;

    // GUI界面

    QThread *workThread;
    DevicesControl *devicesControl;

    // 数据显示
    void resizeEvent(QResizeEvent * event);
    STGraph *stGraph_HSpeed;

    double minDetectRange;      // 垂直向 最小探测距离
    double rangeResol;          // 垂直向 距离分辨率

    //参数配置
    SOFTWARESETTINGS mysetting;
    SettingFile *m_setfile;

    QTimer *TestTimer;
    QDockWidget *dock1,*dock2,*dock3,*dock4; //分别为时空图窗口，水平风速窗口，水平风向窗口，垂直风窗口
    QPushButton *compassbutton,*adqbutton,*motorbutton,*seedbutton,*pulsebutton;
    LineChart *hSpeedChart,*vSpeedLineChart;
    polarchart *hAngleChart;
    barchart *vSpeedBarChart;
    QChartView *hSpeedChartView,*hAngleChartView,*vSpeedChartView,*vSpeedChart2View;
    QAction *startAction,*stopAction,*setAction,*quitAction,
            *hVelocityAction,*hAngleAction,*vVelocityAngleAction,*hDataAction1,*hDataAction2,*vVelocityAngleAction2;

    QMenu *startMenu,*setMenu,*quitMenu,*showMenu,*hDataActionMenu,*vVelocityAngleMenu;
    UserToolBar *toolbar;
    QFont myfont;
    QLabel *statusBarText;
    bool stopNow;
};

#endif // MAINWINDOW_H
