#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);    //֧�ָ߷���
    QApplication a(argc, argv);
    MainWindow  w; // ��ִ�й��캯��
    w.setWindowTitle(QString::fromLocal8Bit("�����糡"));
    QFont font = w.font();
    font.setPointSize(14);
    w.setFont(font);
    w.setMinimumSize(1262,745);
    w.show();

//    w.showMaximized();
//    w.showFullScreen();

    return a.exec();

}