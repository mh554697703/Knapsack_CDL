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
    void size_changed();                    // ���ڵ���ʵʱ������ʾ��widget�߶�
    void TestSignal(const QString s);

private slots:
    void setActionTriggered();			//����
    void startActionTriggered();          //����
    void stopActionTriggered();          //ֹͣ
    void quitActionTriggered();           //�˳�
    void hVelocityActionTriggered(const bool a);
    void hAngleActionTriggered(const bool a);
    void vVelocityAngleActionTriggered(const bool a);
    void hDataAction1Triggered(const bool a);
    void hDataAction2Triggered(const bool a);
    void vVelocityAngleAction2Triggered(const bool a);
    // GUI�������
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

    // GUI����

    QThread *workThread;
    DevicesControl *devicesControl;

    // ������ʾ
    void resizeEvent(QResizeEvent * event);
    STGraph *stGraph_HSpeed;

    double minDetectRange;      // ��ֱ�� ��С̽�����
    double rangeResol;          // ��ֱ�� ����ֱ���

    //��������
    SOFTWARESETTINGS mysetting;
    SettingFile *m_setfile;

    QTimer *TestTimer;
    QDockWidget *dock1,*dock2,*dock3,*dock4; //�ֱ�Ϊʱ��ͼ���ڣ�ˮƽ���ٴ��ڣ�ˮƽ���򴰿ڣ���ֱ�細��
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
