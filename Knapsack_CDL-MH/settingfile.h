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

    //�����ļ�
    void writeSettings(const SOFTWARESETTINGS &setting);      //д���ļ���
    SOFTWARESETTINGS readSettings();                                //���ļ��ж�ȡ
    bool isSettingsChanged(const SOFTWARESETTINGS &setting);  //���ô��ڲ����޸ĺ�δ����ʱ��ʾ�Ƿ񱣴�
    void setUserIniFilePath(QString userIniFilePath);
    QString getUserIniFilePath();
    QString getDefaultIniFilePath();

private:
    SOFTWARESETTINGS fsetting;
    QString userIniFilePath;
    QString defaultIniFilePath;

    void testSettings();                                //��������ļ��Ƿ���ڣ�����������
};

#endif // SETTINGFILE_H
