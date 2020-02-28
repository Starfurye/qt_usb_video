#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <QMainWindow>
#include <QApplication>
#include <QFileDialog>
#include <QImage>
#include <QTimer>
#include <QtPrintSupport/QPrintDialog>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/flann/flann.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"

#include "win_qextserialport.h"

#include <winioctl.h>
#include <initguid.h>

//#include <setupapi.h>
//#include <windows.h>
//#include <legacy_stdio_definitions.h>

#include "inputdialog.h"
#include "modifydialog.h"
#include "getalldialog.h"
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //QCustomPlot画图初始化
    void initRealtimePlot();
    //添加数据点
    void addData(double r, double g, double b, double x);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::MainWindow *ui;

    inputDialog *dialog;
    modifyDialog *mddialog;
    getallDialog *alldialog;

    cv::Mat frame;
    cv::VideoCapture capture;
    cv::VideoWriter writer;   //make a video re

    QImage  image;
    QTimer *timer;
    double rate; //FPS

    Win_QextSerialPort *myCom;

    QVector<QPoint> Points;   //存放鼠标点击位置相对于QLabel的坐标
    int pointNumber;          //能够选择的像素点数目(<=10)

    QVector<double> xVector;  //framenubmer
    QVector<double> rValue;   //RGB色彩的均值
    QVector<double> gValue;
    QVector<double> bValue;

private slots:
    void nextFrame();
    void readMyCom();

    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

    void on_action_3_triggered();
    void on_action_4_triggered();
    void on_action_triggered();

    void on_openButton_clicked();
    void on_closeButton_clicked();
    void on_sendMsgButton_clicked();
};

#endif // MAINWINDOW_HPP
