#include "parametersetdialog.h"
#include "ui_parametersetdialog.h"
#include "mainwindow.h"

ParameterSetDialog::ParameterSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterSetDialog)
{
    ui->setupUi(this);
    paraSettingFile = new SettingFile();
}

ParameterSetDialog::~ParameterSetDialog()
{
    delete paraSettingFile;
    delete ui;
}

SOFTWARESETTINGS ParameterSetDialog::getParaSettings()
{
    paraSettings.laserPulseEnergy = ui->laserPulseEnergydoubleSpinBox->value();
    paraSettings.laserLocalPower = ui->laserLocalPowerdoubleSpinBox->value();
    paraSettings.laserAOMFreq = ui->laserAOMdoubleSpinBox->value();
    paraSettings.laserPulseWidth = ui->laserPulseWidthSpinBox->value();
    paraSettings.laserRPF = ui->laserRPFSpinBox->value();
    paraSettings.laserWaveLength = ui->laserWaveLengthdoubleSpinBox->value();

    paraSettings.detectMode = ui->detectModeComboBox->currentIndex();
    paraSettings.elevationAngle = ui->elevationAngledoubleSpinBox->value();
    paraSettings.azAngleStart = ui->azAngleStartdoubleSpinBox->value();
    paraSettings.azAngleStep = ui->azAngleStepdoubleSpinBox->value();
    paraSettings.angleNum = ui->angleNumSpinBox->value();
    paraSettings.circleNum = ui->circleNumdoubleSpinBox->value();
    paraSettings.isAngleChecked = ui->anglekeyRadioButton->isChecked();
    paraSettings.isCircleChecked = ui->circlekeyRadioButton->isChecked();
    paraSettings.motorSpeed = ui->motorSpeeddoubleSpinBox->value();
    paraSettings.intervalTime = ui->intervalTimedoubleSpinBox->value();
    paraSettings.groupTime = ui->groupTimedoubleSpinBox->value();

    paraSettings.sampleFreq = ui->sampleFreqComboBox->currentText().toDouble();
    paraSettings.triggerLevel = ui->triggerLevelSpinBox->value();
    paraSettings.nPointsPreTrigger = ui->nPointsPretriggerSpinBox->value();
    paraSettings.nPulsesAcc = ui->nPulsesAccSpinBox->value();
    paraSettings.nRangeBin = ui->nRangeBinSpinBox->value();
    paraSettings.nPointsPerBin = ui->nPointsPerBinSpinBox->value();
    paraSettings.nPointsMirrorWidth = ui->nPointsMirrorWidthSpinBox->value();
    paraSettings.overlapRatio = ui->overlapRatioComboBox->currentText().toDouble();

    paraSettings.nPointsObjFreq = getObjFreqPoints(ui->nPointsObjFreqdoubleSpinBox->value());
    paraSettings.nDirsVectorCal = ui->nDirsVectorCalSpinBox->value();

    paraSettings.dataFilePath = ui->dataFilePathLineEdit->text();
    paraSettings.autoCreateDateDir = ui->autoCreateDateDirCheckBox->isChecked();
    paraSettings.nDirsPerFile = ui->nDirsPerFileSpinBox->value();

    return paraSettings;
}

void ParameterSetDialog::setParaSettings(SOFTWARESETTINGS settings, bool isWorking)
{
    paraSettings = settings;
    refreshDisp();
}

void ParameterSetDialog::refreshDisp()
{
    // laser parameter refresh display on textchanged
    ui->laserPulseEnergydoubleSpinBox->setValue(QString::number(paraSettings.laserPulseEnergy).toDouble());
    ui->laserLocalPowerdoubleSpinBox->setValue(QString::number(paraSettings.laserLocalPower).toDouble());
    ui->laserAOMdoubleSpinBox->setValue(QString::number(paraSettings.laserAOMFreq).toDouble());
    ui->laserPulseWidthSpinBox->setValue(QString::number(paraSettings.laserPulseWidth).toInt());
    ui->laserRPFSpinBox->setValue(QString::number(paraSettings.laserRPF).toDouble());
    ui->laserWaveLengthdoubleSpinBox->setValue(QString::number(paraSettings.laserWaveLength).toDouble());

    // scan parameter refresh display on text changed, classified by detect mode
    ui->detectModeComboBox->setCurrentIndex(paraSettings.detectMode);
    if (paraSettings.detectMode == 0) {
        // single group detection
        ui->anglekeyRadioButton->setEnabled(true);
        ui->circlekeyRadioButton->setEnabled(true);
        ui->angleNumSpinBox->setEnabled(paraSettings.isAngleChecked);
        ui->circleNumdoubleSpinBox->setEnabled(paraSettings.isCircleChecked);
        ui->intervalTimedoubleSpinBox->setEnabled(false);
        ui->groupTimedoubleSpinBox->setEnabled(false);

        ui->angleNumSpinBox->setValue(QString::number(paraSettings.angleNum).toInt());
        ui->circleNumdoubleSpinBox->setValue(QString::number(paraSettings.circleNum).toDouble());
        ui->anglekeyRadioButton->setChecked(paraSettings.isAngleChecked);
        ui->circlekeyRadioButton->setChecked(paraSettings.isCircleChecked);

    }
    else if (paraSettings.detectMode == 1) {
        // continous detection
        ui->anglekeyRadioButton->setEnabled(false);
        ui->angleNumSpinBox->setEnabled(false);
        ui->circlekeyRadioButton->setEnabled(false);
        ui->circleNumdoubleSpinBox->setEnabled(false);
        ui->intervalTimedoubleSpinBox->setEnabled(false);
        ui->groupTimedoubleSpinBox->setEnabled(false);

        //        ui->angleNumLineEdit->setText(QString::fromLocal8Bit("不可用"));
        //        ui->circleNumLineEdit->setText(QString::fromLocal8Bit("不可用"));
        ui->anglekeyRadioButton->setChecked(false);
        ui->circlekeyRadioButton->setChecked(false);
    }
    else if (paraSettings.detectMode == 2) {
        // scheduled detection
        ui->anglekeyRadioButton->setEnabled(false);
        ui->angleNumSpinBox->setEnabled(false);
        ui->circlekeyRadioButton->setEnabled(false);
        ui->circleNumdoubleSpinBox->setEnabled(false);
        ui->intervalTimedoubleSpinBox->setEnabled(true);
        ui->groupTimedoubleSpinBox->setEnabled(true);

        //        ui->angleNumLineEdit->setText(QString::fromLocal8Bit("不可用"));
        //        ui->circleNumLineEdit->setText(QString::fromLocal8Bit("不可用"));
        ui->anglekeyRadioButton->setChecked(false);
        ui->circlekeyRadioButton->setChecked(false);
    }

    ui->elevationAngledoubleSpinBox->setValue(QString::number(paraSettings.elevationAngle).toDouble());
    ui->azAngleStartdoubleSpinBox->setValue(QString::number(paraSettings.azAngleStart).toDouble());
    ui->azAngleStepdoubleSpinBox->setValue(QString::number(paraSettings.azAngleStep).toDouble());

    ui->motorSpeeddoubleSpinBox->setValue(QString::number(paraSettings.motorSpeed).toDouble());
    ui->intervalTimedoubleSpinBox->setValue(QString::number(paraSettings.intervalTime).toDouble());
    ui->groupTimedoubleSpinBox->setValue(QString::number(paraSettings.groupTime).toDouble());


    ui->sampleFreqComboBox->setCurrentText(QString::number(paraSettings.sampleFreq));
    ui->triggerLevelSpinBox->setValue(QString::number(paraSettings.triggerLevel).toInt());
    QString voltageLevelStr = "=" + QString::number(getVoltageTriggerLevel(), 'f', 3) + "V";
    ui->triggerLevelInVoltageDispLabel->setText(voltageLevelStr);
    ui->nPointsPretriggerSpinBox->setValue(QString::number(paraSettings.nPointsPreTrigger).toInt());
    QString preTriggerWidthTimeStr = "=" + QString::number(getPreTriggerTimeLength(), 'f', 3) + "ns";
    ui->preTriggerInTimeDispLabel->setText(preTriggerWidthTimeStr);
    ui->nPointsMirrorWidthSpinBox->setValue(QString::number(paraSettings.nPointsMirrorWidth).toInt());
    QString mirrorWidthTimeStr = "=" + QString::number(getMirrorWidthTime(), 'f', 3) + "ns";
    ui->mirrorWidthTimeDispLabel->setText(mirrorWidthTimeStr);
    ui->nPointsPerBinSpinBox->setValue(QString::number(paraSettings.nPointsPerBin).toInt());

    ui->overlapRatioComboBox->setCurrentText(QString::number(paraSettings.overlapRatio));

    ui->nRangeBinSpinBox->setValue(QString::number(paraSettings.nRangeBin).toInt());

    ui->nPulsesAccSpinBox->setValue(QString::number(paraSettings.nPulsesAcc).toInt());

    double objVelocity = getObjVelocity();
    ui->nPointsObjFreqdoubleSpinBox->setValue(QString::number(objVelocity).toDouble());
    QString objFreqPointsDispStr = "m/s=" + QString::number(paraSettings.nPointsObjFreq) +
            QString::fromLocal8Bit("点");
    ui->objFreqPointsDispLabel->setText(objFreqPointsDispStr);
    ui->nDirsVectorCalSpinBox->setValue(QString::number(paraSettings.nDirsVectorCal).toInt());

    checkDataFilePath();
    ui->dataFilePathLineEdit->setText(paraSettings.dataFilePath);
    QDir currentDir(paraSettings.dataFilePath);
    if (!currentDir.exists())
        ui->dataFilePathLineEdit->setStyleSheet("color:red");
    else
        ui->dataFilePathLineEdit->setStyleSheet("color:black");
    ui->dataFilePathLineEdit->setFont(QFont("Microsoft Yahei"));
    ui->autoCreateDateDirCheckBox->setChecked(paraSettings.autoCreateDateDir);
    ui->nDirsPerFileSpinBox->setValue(QString::number(paraSettings.nDirsPerFile).toInt());

    //右侧状态栏信息
    //最小探测距离
    double minDetectRange = getMinDetectRange();
    QString minDetectRangeDispStr = QString::number(minDetectRange, 'f', 2) + "m" + "/" +
            QString::number(minDetectRange*qSin(qDegreesToRadians(paraSettings.elevationAngle)), 'f', 2) + "m";
    ui->startDetectRangeDispLineEdit->setText(minDetectRangeDispStr);

    //最大探测距离
    double maxDetctRange = getMaxDetectRange();
    QString maxDetctRangeDispStr = QString::number(maxDetctRange, 'f', 2) + "m" + "/" +
            QString::number(maxDetctRange*qSin(qDegreesToRadians(paraSettings.elevationAngle)), 'f', 2) + "m";
    ui->maxDetectRangeDispLineEdit->setText(maxDetctRangeDispStr);

    //距离分辨率
    double rangeResol = getRangeResolution();
    QString rangeResolDispStr = QString::number(rangeResol, 'f', 2) + "m" + "/" +
            QString::number(rangeResol*qSin(qDegreesToRadians(paraSettings.elevationAngle)), 'f', 2) + "m";
    ui->rangeResoDispLineEdit->setText(rangeResolDispStr);

    QFileInfo userIniFileInfo(paraSettingFile->getUserIniFilePath());
    ui->settingFilePathDispLineEdit->setText(userIniFileInfo.fileName());
    if (!userIniFileInfo.exists()) {
        ui->settingFilePathDispLineEdit->setStyleSheet("color:red");
    }
    else {
        ui->settingFilePathDispLineEdit->setStyleSheet("color:black");
    }
    ui->settingFilePathDispLineEdit->setFont(QFont("Microsoft Yahei"));
    ui->settingFilePathDispLineEdit->setToolTip(paraSettingFile->getUserIniFilePath());
}

uint ParameterSetDialog::getObjFreqPoints(double losVelocityRange)
{
    double objFreqPoints = losVelocityRange/(paraSettings.sampleFreq/nFFT*paraSettings.laserWaveLength/2);
    objFreqPoints = qRound(objFreqPoints);
    return objFreqPoints;
}

double ParameterSetDialog::getObjVelocity()
{
    double objVelocity = paraSettings.sampleFreq/nFFT*paraSettings.laserWaveLength/2*paraSettings.nPointsObjFreq;
    return objVelocity;
}

double ParameterSetDialog::getVoltageTriggerLevel()
{
    double voltageLevel = 1.1*paraSettings.triggerLevel/qPow(2,13);
    return voltageLevel;
}

double ParameterSetDialog::getPreTriggerTimeLength()
{
    double widthInTime = 1/paraSettings.sampleFreq*1000*paraSettings.nPointsPreTrigger;
    return widthInTime;
}

double ParameterSetDialog::getMirrorWidthTime()
{
    double sampleFreq = paraSettings.sampleFreq;
    int mirrorWidthPoints = paraSettings.nPointsMirrorWidth;
    double mirrorWidthTime = mirrorWidthPoints/sampleFreq*1000;
    return mirrorWidthTime;
}

double ParameterSetDialog::getRangeResolution()
{
    double overlapRatio = paraSettings.overlapRatio;
    int pointsPerBin = paraSettings.nPointsPerBin;
    double sampleFreq = paraSettings.sampleFreq;
    double resol = lightSpeed/sampleFreq/1000000/2;
    double rangeResol = resol*(pointsPerBin*(1-overlapRatio));
    return rangeResol;
}

double ParameterSetDialog::getMinDetectRange()
{
    int pointsPerBin = paraSettings.nPointsPerBin;
    int mirrorWidthPoints = paraSettings.nPointsMirrorWidth;
    double sampleFreq = paraSettings.sampleFreq;
    double resol = lightSpeed/sampleFreq/1000000/2;
    double minDetectRange = resol*(mirrorWidthPoints+pointsPerBin/2);
    return minDetectRange;
}

double ParameterSetDialog::getMaxDetectRange()
{
    int pointsPerBin = paraSettings.nPointsPerBin;
    int mirrorWidthPoints = paraSettings.nPointsMirrorWidth;
    int nRangeBin = paraSettings.nRangeBin;
    double sampleFreq = paraSettings.sampleFreq;
    double resol = lightSpeed/sampleFreq/1000000/2;
    double maxDetectRange = resol*(mirrorWidthPoints+pointsPerBin+
                                   pointsPerBin*(nRangeBin-1)-pointsPerBin/2);
    return maxDetectRange;
}

void ParameterSetDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << event->localPos();
    qDebug() << ui->dataFilePathLineEdit->x();
    qDebug() << ui->dataFilePathLineEdit->y();
    qDebug() << event->FocusIn;
}

void ParameterSetDialog::on_sampleFreqComboBox_activated(const QString &arg1)
{
    paraSettings.sampleFreq = arg1.toDouble();
    refreshDisp();
    //    DispSettings disp(paraSettings);
}

void ParameterSetDialog::on_overlapRatioComboBox_activated(const QString &arg1)
{
    paraSettings.overlapRatio = arg1.toDouble();
    refreshDisp();
}

void ParameterSetDialog::on_detectModeComboBox_activated(int index)
{
    paraSettings.detectMode = index;
    refreshDisp();
}

void ParameterSetDialog::on_anglekeyRadioButton_clicked(bool checked)
{
    paraSettings.isAngleChecked = checked;
    paraSettings.isCircleChecked = !checked;
    refreshDisp();
}

void ParameterSetDialog::on_circlekeyRadioButton_clicked(bool checked)
{
    paraSettings.isCircleChecked = checked;
    paraSettings.isAngleChecked = !checked;
    refreshDisp();
}

void ParameterSetDialog::on_autoCreateDateDirCheckBox_clicked(bool checked)
{
    paraSettings.autoCreateDateDir = checked;
    refreshDisp();
}

void ParameterSetDialog::on_pathModifyPushButton_clicked()
{
    qDebug() << paraSettings.dataFilePath;
    QString newPath = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("设置数据保存路径"),
                                                        paraSettings.dataFilePath,
                                                        QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
    if (!newPath.isEmpty()) {
        paraSettings.dataFilePath = newPath;
    }
    refreshDisp();
}

void ParameterSetDialog::on_saveSettingsPushButton_clicked()
{
    QString defaultSettingFilePath = paraSettingFile->getDefaultIniFilePath();
    QString userSettingFilePath = paraSettingFile->getUserIniFilePath();
    QDir userSettingFile(userSettingFilePath);
    if (defaultSettingFilePath == userSettingFilePath &&
            !userSettingFile.exists()) {
        userSettingFilePath = QFileDialog::getSaveFileName(this,
                                                           QString::fromLocal8Bit("另存为设置文件"),
                                                           paraSettings.dataFilePath, tr("*.ini"));

        if (!userSettingFilePath.isEmpty()) {
            if (!userSettingFilePath.endsWith(".ini", Qt::CaseInsensitive)) {
                userSettingFilePath.append(".ini");
            }
            qDebug() << userSettingFilePath;
            paraSettingFile->setUserIniFilePath(userSettingFilePath);
            paraSettingFile->writeSettings(paraSettings);
        }
    }
    else {
        paraSettingFile->writeSettings(paraSettings);
    }
    refreshDisp();
}

void ParameterSetDialog::on_saveasSettingsPushButton_clicked()
{
    QString userIniFileName = QFileDialog::getSaveFileName(this,
                                                           QString::fromLocal8Bit("另存为设置文件"),
                                                           paraSettings.dataFilePath, tr("*.ini"));
    if (!userIniFileName.isEmpty()) {
        if (!userIniFileName.endsWith(".ini", Qt::CaseInsensitive)) {
            userIniFileName.append(".ini");
        }
        paraSettingFile->setUserIniFilePath(userIniFileName);
        paraSettingFile->writeSettings(paraSettings);
    }
//    refreshDisp();
}

void ParameterSetDialog::on_loadSettingsPushButton_clicked()
{
    QString userIniFileName = QFileDialog::getOpenFileName(this,
                                                           QString::fromLocal8Bit("加载设置文件"),
                                                           paraSettings.dataFilePath, tr("*.ini"));
    if (!userIniFileName.isEmpty()) {
        qDebug() << userIniFileName;
        paraSettingFile->setUserIniFilePath(userIniFileName);
        paraSettings = paraSettingFile->readSettings();
        refreshDisp();
    }
}

void ParameterSetDialog::on_confirmSetPushButton_clicked()
{
    accept();
}

void ParameterSetDialog::on_cancelSetPushButton_clicked()
{
    reject();
}

void ParameterSetDialog::on_resetSettingsPushButton_clicked()
{
    QMessageBox::StandardButton rb;
    rb = QMessageBox::question(this, QString::fromLocal8Bit("重置为默认设置？"),
                               QString::fromLocal8Bit("此操作将会重置所有设置信息为默认设置，请三思！！"),
                               QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (rb == QMessageBox::Yes) {
        qDebug() << "reset default";
        paraSettingFile->setUserIniFilePath(paraSettingFile->getDefaultIniFilePath());
        paraSettings = paraSettingFile->readSettings();
        refreshDisp();
        DispSettings disp(paraSettings);
    }
}

void ParameterSetDialog::on_laserLocalPowerdoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.laserLocalPower = arg1;
}

void ParameterSetDialog::on_laserPulseEnergydoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.laserPulseEnergy = arg1;
}

void ParameterSetDialog::on_laserRPFdoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.laserRPF = arg1;
//    refreshDisp();
}

void ParameterSetDialog::on_laserPulseWidthdoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.laserPulseWidth = arg1;
}

void ParameterSetDialog::on_laserWaveLengthdoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.laserWaveLength = arg1;
//    refreshDisp();
}

void ParameterSetDialog::on_laserAOMdoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.laserAOMFreq = arg1;
}


void ParameterSetDialog::on_triggerLevelSpinBox_valueChanged(int arg1)
{
    paraSettings.triggerLevel = arg1;
    //    DispSettings disp(paraSettings);
    refreshDisp();
}



void ParameterSetDialog::on_nPointsPretriggerSpinBox_valueChanged(int arg1)
{
    paraSettings.nPointsPreTrigger = arg1;
    //    DispSettings disp(paraSettings);
    refreshDisp();
}

void ParameterSetDialog::on_nPointsMirrorWidthSpinBox_valueChanged(int arg1)
{
    paraSettings.nPointsMirrorWidth = arg1;
    refreshDisp();
}

void ParameterSetDialog::on_nPointsPerBinSpinBox_valueChanged(int arg1)
{
    paraSettings.nPointsPerBin = arg1;
    if(paraSettings.nPointsPerBin%2 == 1) {
        paraSettings.nPointsPerBin =  paraSettings.nPointsPerBin + 1;
    }
    refreshDisp();
}

void ParameterSetDialog::on_nPulsesAccSpinBox_valueChanged(int arg1)
{
    paraSettings.nPulsesAcc = arg1;
    refreshDisp();
}

void ParameterSetDialog::on_nRangeBinSpinBox_valueChanged(int arg1)
{
    paraSettings.nRangeBin = arg1;
    refreshDisp();
}


void ParameterSetDialog::on_nPointsObjFreqdoubleSpinBox_valueChanged(double arg1)
{
    qDebug() << "Changed Obj Freq";
    paraSettings.nPointsObjFreq = getObjFreqPoints(arg1);
    QString objFreqPointsDispStr = "m/s=" + QString::number(paraSettings.nPointsObjFreq) +
            QString::fromLocal8Bit("点");
    ui->objFreqPointsDispLabel->setText(objFreqPointsDispStr);
    ui->nPointsObjFreqdoubleSpinBox->setValue(getObjVelocity());
}

void ParameterSetDialog::on_nPointsObjFreqdoubleSpinBox_editingFinished()
{
//    refreshDisp();
}

void ParameterSetDialog::on_nDirsVectorCalSpinBox_valueChanged(int arg1)
{
    paraSettings.nDirsVectorCal = arg1;
//    refreshDisp();
}


void ParameterSetDialog::on_azAngleStartdoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.azAngleStart = arg1;
}

void ParameterSetDialog::on_azAngleStepdoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.azAngleStep = arg1;
    if (paraSettings.detectMode == 0) {
        if (paraSettings.isAngleChecked) {
            paraSettings.circleNum = paraSettings.angleNum/(360.0/paraSettings.azAngleStep);
        }
        else if(paraSettings.isCircleChecked){
            paraSettings.angleNum = paraSettings.circleNum*(360.0/paraSettings.azAngleStep);
        }
    }
    refreshDisp();
}

void ParameterSetDialog::on_motorSpeeddoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.motorSpeed = arg1;
    refreshDisp();
}

void ParameterSetDialog::on_elevationAngledoubleSpinBox_valueChanged(double arg1)
{
    paraSettings.elevationAngle = arg1;
    refreshDisp();
}

void ParameterSetDialog::on_intervalTimedoubleSpinBox_valueChanged(double arg1)
{
    if (paraSettings.detectMode == 2) {
        paraSettings.intervalTime = arg1;
    }
    refreshDisp();
}

void ParameterSetDialog::on_groupTimedoubleSpinBox_valueChanged(double arg1)
{
    if (paraSettings.detectMode == 2) {
        paraSettings.groupTime = arg1;
    }
    refreshDisp();
}

void ParameterSetDialog::on_nDirsPerFileSpinBox_valueChanged(int arg1)
{
    paraSettings.nDirsPerFile = arg1;
}

void ParameterSetDialog::on_angleNumSpinBox_valueChanged(int arg1)
{
    if (paraSettings.detectMode == 0) {
        if (paraSettings.isAngleChecked) {
            paraSettings.angleNum = arg1;
            paraSettings.circleNum = paraSettings.angleNum/(360.0/paraSettings.azAngleStep);
            refreshDisp();
            ui->circleNumdoubleSpinBox->setValue(QString::number(paraSettings.circleNum).toDouble());
        }
    }
}

void ParameterSetDialog::on_circleNumdoubleSpinBox_valueChanged(double arg1)
{
    if (paraSettings.detectMode == 0) {
        if (paraSettings.isCircleChecked) {
            paraSettings.circleNum = arg1;
            paraSettings.angleNum = paraSettings.circleNum*360.0/paraSettings.azAngleStep;
            refreshDisp();
            ui->angleNumSpinBox->setValue(QString::number(paraSettings.angleNum).toInt());
        }
    }
}

void ParameterSetDialog::checkDataFilePath()
{
    QString str = paraSettings.dataFilePath;

    QDir mypath(str);
    QString dirname = mypath.dirName();
    QDateTime time = QDateTime::currentDateTime();

    int num = dirname.toInt();
    int len = dirname.length();
    if(paraSettings.autoCreateDateDir)
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

    paraSettings.dataFilePath = str;
}
