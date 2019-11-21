#include "mainwindow.hpp"
#include <QApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>

QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::addLibraryPath("./plugins");

    // build and access to database

    database.setDatabaseName("./qtusb.db");
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        qDebug() << "Succeed to connect database." ;
    }

    MainWindow w;
    w.setWindowTitle("LPI诊断系统");
    w.show();
    qDebug()<<"debug";
    return a.exec();
}
