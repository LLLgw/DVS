#include "mainwindow.h"

#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<QVector<quint16>>>("QVector<QVector<quint16>>");
    qRegisterMetaType<QVector<quint16>>("QVector<quint16>");
    MainWindow w;
    w.show();
    return a.exec();
}
