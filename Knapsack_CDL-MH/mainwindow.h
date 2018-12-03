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
#include <QMouseEvent>
#include <QKeyEvent>
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
    void hVelocityAngleActionTriggered(const bool a);
    void vVelocityAngleActionTriggered(const bool a);
    void hDataActionTriggered(const bool a);
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
    void on_CompassButton_clicked();

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
    QDockWidget *dock1,*dock2,*dock3; //�ֱ�Ϊ��ʷ���ݴ��ڣ�ˮƽ�細�ڣ���ֱ�細�ڡ�
    QPushButton *compassbutton,*adqbutton,*motorbutton,*seedbutton,*pulsebutton;
    Chart *hSpeedChart;
    polarchart *hAngleChart;
    barchart *vSpeedChart;
    QAction *startAction,*stopAction,*setAction,*quitAction,
            *hVelocityAngleAction,*vVelocityAngleAction,*hDataAction;

    QMenu *startMenu,*stopMenu,*setMenu,*quitMenu,*showMenu;
    UserToolBar *toolbar;
    QFont myfont;
    QLabel *statusBarText;
    bool stopNow;
};

#endif // MAINWINDOW_H
