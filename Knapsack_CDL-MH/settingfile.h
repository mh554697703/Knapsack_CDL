#ifndef SETTINGFILE_H
#define SETTINGFILE_H
#include <QString>
#include <QSettings>
#include <QDebug>
#include <QtCore>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include "global_defines.h"

class SettingFile
{
public:
    SettingFile();

    //配置文件
    void writeSettings(const SOFTWARESETTINGS &setting);      //写入文件中
    SOFTWARESETTINGS readSettings();                                //从文件中读取
    bool isSettingsChanged(const SOFTWARESETTINGS &setting);  //设置窗口参数修改后，未保存时提示是否保存
    void setUserIniFilePath(QString userIniFilePath);
    QString getUserIniFilePath();
    QString getDefaultIniFilePath();

private:
    SOFTWARESETTINGS fsetting;
    QString userIniFilePath;
    QString defaultIniFilePath;

    void testSettings();                                //检查配置文件是否存在，若无则生成
};

#endif // SETTINGFILE_H
