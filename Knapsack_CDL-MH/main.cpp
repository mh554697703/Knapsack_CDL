#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);    //支持高分屏
    QApplication a(argc, argv);
    MainWindow  w; // 先执行构造函数
    w.setWindowTitle(QString::fromLocal8Bit("大气风场"));
    QFont font = w.font();
    font.setPointSize(14);
    w.setFont(font);
    w.setMinimumSize(1027,768);
    w.show();

//    w.showMaximized();
//    w.showFullScreen();

    return a.exec();

}
