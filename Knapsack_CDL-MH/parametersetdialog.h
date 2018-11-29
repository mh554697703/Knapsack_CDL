#ifndef PARAMETERSETDIALOG_H
#define PARAMETERSETDIALOG_H
#include "global_defines.h"
#include "settingfile.h"
#include <QFileDialog>
#include <QDialog>
#include <QMessageBox>
#include <dispsettings.h>
#include <QRegExp>
#include <QRegExpValidator>

namespace Ui {
class ParameterSetDialog;
}

class ParameterSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterSetDialog(QWidget *parent = nullptr);
    ~ParameterSetDialog();
    SOFTWARESETTINGS getParaSettings();
    void setParaSettings(SOFTWARESETTINGS settings, bool isWorking);

private slots:

    void on_sampleFreqComboBox_activated(const QString &arg1);

    void on_overlapRatioComboBox_activated(const QString &arg1);

    void on_detectModeComboBox_activated(int index);

    void on_anglekeyRadioButton_clicked(bool checked);

    void on_circlekeyRadioButton_clicked(bool checked);

    void on_autoCreateDateDirCheckBox_clicked(bool checked);

    void on_pathModifyPushButton_clicked();

    void on_saveSettingsPushButton_clicked();

    void on_saveasSettingsPushButton_clicked();

    void on_loadSettingsPushButton_clicked();

    void on_confirmSetPushButton_clicked();

    void on_cancelSetPushButton_clicked();

    void on_resetSettingsPushButton_clicked();

    void on_laserLocalPowerdoubleSpinBox_valueChanged(double arg1);

    void on_laserPulseEnergydoubleSpinBox_valueChanged(double arg1);

    void on_laserRPFdoubleSpinBox_valueChanged(double arg1);

    void on_laserPulseWidthdoubleSpinBox_valueChanged(double arg1);

    void on_laserWaveLengthdoubleSpinBox_valueChanged(double arg1);

    void on_laserAOMdoubleSpinBox_valueChanged(double arg1);

    void on_triggerLevelSpinBox_valueChanged(int arg1);

    void on_nPointsPretriggerSpinBox_valueChanged(int arg1);

    void on_nPointsMirrorWidthSpinBox_valueChanged(int arg1);

    void on_nPointsPerBinSpinBox_valueChanged(int arg1);

    void on_nPulsesAccSpinBox_valueChanged(int arg1);

    void on_nRangeBinSpinBox_valueChanged(int arg1);

    void on_nPointsObjFreqdoubleSpinBox_valueChanged(double arg1);

    void on_nPointsObjFreqdoubleSpinBox_editingFinished();

    void on_nDirsVectorCalSpinBox_valueChanged(int arg1);

    void on_azAngleStartdoubleSpinBox_valueChanged(double arg1);

    void on_azAngleStepdoubleSpinBox_valueChanged(double arg1);

    void on_motorSpeeddoubleSpinBox_valueChanged(double arg1);

    void on_elevationAngledoubleSpinBox_valueChanged(double arg1);

    void on_intervalTimedoubleSpinBox_valueChanged(double arg1);

    void on_groupTimedoubleSpinBox_valueChanged(double arg1);

    void on_nDirsPerFileSpinBox_valueChanged(int arg1);

    void on_angleNumSpinBox_valueChanged(int arg1);

    void on_circleNumdoubleSpinBox_valueChanged(double arg1);

    void checkDataFilePath();
private:
    Ui::ParameterSetDialog *ui;
    void refreshDisp();
    SOFTWARESETTINGS paraSettings;
    uint getObjFreqPoints(double losVelocityRange);
    double getObjVelocity();
    double getVoltageTriggerLevel();
    double getPreTriggerTimeLength();
    double getMirrorWidthTime();
    double getRangeResolution();
    double getMinDetectRange();
    double getMaxDetectRange();
    SettingFile *paraSettingFile;

    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // PARADIALOG_H
