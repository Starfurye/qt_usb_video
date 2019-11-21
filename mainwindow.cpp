#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "inputdialog.h"

#include <iostream>

#include <gl/GLU.h>
// #include <windows.h>
#include <QPixmap>
#include <QDebug>
#include <QMouseEvent>

#include "bulkloop/inc/CyAPI.h"
#include "bulkloop/inc/cyioctl.h"
#include "bulkloop/inc/CyUSB30_def.h"
#include "bulkloop/inc/UsbdStatus.h"
#include "bulkloop/inc/usb200.h"
#include "bulkloop/inc/VersionNo.h"

using namespace cv;

static const int g_nKwidthTrackBarMaxValue = 9;
static const int g_nKheightTrackBarMaxValue = 9;
static const int g_nsigmaXTrackBarMaxValue = 5;
static const int g_nsigmaYTrackBarMaxvalue = 5;

//定义每个轨迹条的初始值
static int g_nKwidthTrackBarValue = 1;
static int g_nKheightTrackBarValue = 1;
static int g_nsigmaXTrackBarValue = 1;
static int g_nsigmaYTrackBarValue = 1;

//--高斯核的宽度和高度
static int g_kernelWidthValue;
static int g_kernelHeightValue;

//--高斯模糊的输入和输出图像
static Mat srcImage;
static Mat dstImage;

//播放视频时的帧数
static int framenumber;

//绘制点的最大个数
static int MAX_NUM = 50;
static int plotSize = 0;

//外部函数声明
void on_GaussianBlurTrackbar(int, void*);
QImage Mat2QImage(const Mat &inputMat);

/*MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    VideoCapture cam(1);
    if (!cam.isOpened()) exit(0);
     namedWindow("摄像头");
     Mat rgb;
     QImage img;


    while (true)
    {
        cam >> input_image;

        namedWindow("原图像", WINDOW_NORMAL); //定义窗口显示属性
        imshow("原图像", input_image);

       namedWindow("高斯滤波图像", WINDOW_NORMAL);

        //定义每个轨迹条名字
        char widthTrackBarName[20];
        sprintf(widthTrackBarName, "核函数width %d", g_nKwidthTrackBarMaxValue);

        char heightTrackBarName[20];
        sprintf(heightTrackBarName, "核函数height %d", g_nKheightTrackBarMaxValue);

        char sigmaXTrackBarName[20];
        sprintf(sigmaXTrackBarName, "核函数sigmaX %d", g_nsigmaXTrackBarMaxValue);

        char sigmaYTrackBarName[20];
        sprintf(sigmaYTrackBarName, "核函数sigmaY %d", g_nsigmaYTrackBarMaxvalue);

        //创建轨迹条
        createTrackbar(widthTrackBarName, "高斯滤波图像", &g_nKwidthTrackBarValue,
            g_nKwidthTrackBarMaxValue, on_GaussianBlurTrackbar);
        on_GaussianBlurTrackbar(g_nKwidthTrackBarValue, 0);

        createTrackbar(heightTrackBarName, "高斯滤波图像", &g_nKheightTrackBarValue,
            g_nKheightTrackBarMaxValue, on_GaussianBlurTrackbar);
        on_GaussianBlurTrackbar(g_nKheightTrackBarValue, 0);

        createTrackbar(sigmaXTrackBarName, "高斯滤波图像", &g_nsigmaXTrackBarValue,
            g_nsigmaXTrackBarMaxValue, on_GaussianBlurTrackbar);
        on_GaussianBlurTrackbar(g_nsigmaXTrackBarValue, 0);

        createTrackbar(sigmaYTrackBarName, "高斯滤波图像", &g_nsigmaYTrackBarValue,
            g_nsigmaYTrackBarMaxvalue, on_GaussianBlurTrackbar);
        on_GaussianBlurTrackbar(g_nsigmaYTrackBarValue, 0);

        //显示效果图
        imshow("高斯滤波[效果图]", dstImage);

        //imshow("input image", input_image);
        if (27 == waitKey(30))
            break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
*/

// 添加患者信息
extern QString addName,addAge,addID;
extern int addSex;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pointNumber = 5; //设置能够选择的像素点数目
    ui->label->installEventFilter(this);//为label安装事件过滤器

    initRealtimePlot(); //QCustomPlot画图初始化

    /////读取串口数据
    struct PortSettings myComSetting = {BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500};
    //定义一个结构体，用来存放串口各个参数
    myCom = new Win_QextSerialPort("com1",myComSetting,QextSerialBase::Polling);
    //定义串口对象，并传递参数，在构造函数里对其进行初始化
    //定义串口对象，指定串口名和查询模式，这里使用Polling
    myCom ->open(QIODevice::ReadWrite);
    //以可读写方式打开串口
    timer=new QTimer(this);
    myCom->setTimeout(10);
    timer->start(200);
    //设置延时为200ms
    connect(timer, SIGNAL(timeout()), this, SLOT(readMyCom()));//Polling模式定时器触发timeout（）信号

    ///// USB3
    CCyFX3Device *m_usbDevice = new CCyFX3Device();
    FX3_FWDWNLOAD_ERROR_CODE dwld_status = FAILED;
    // CCyUSBEndPoint类主要负责对USB端点进行操作，包含获取和设置USB端点的各种描述符和进行数据传输等；
    // CCyBulkEndPoint类主要负责完成USB块传输操作，包含对USB块传输端点的读写操作函数；
    CCyBulkEndPoint *BulkInEpt = nullptr;
    CCyBulkEndPoint *BulkOutEpt = nullptr;
    // Open first USB device
    // Open 函数，用于打开USB设备
    // Close 函数，关闭主机设备和USB驱动的接口
    if(m_usbDevice->Open(0))
    {
        //Check if boot loader is running.
        int status = m_usbDevice->IsBootLoaderRunning();
        if(status)
        {
            // Download the file C:\Bulkloop.img file to FX3 device RAM
            char tm[20]="C:\\Bulkloop.img";
            int dwld_status = m_usbDevice->DownloadFw(tm,RAM);
        }
    }

    //从EndPoints数组中找出块传输端点(由官方例子改编而来)
    int eptCount = m_usbDevice->EndPointCount();//当前设备的接口数+1
    for (int i=1; i<eptCount; i++)
    {
        //bool bIn = ((m_usbDevice->EndPoints[i]->Address & 0x80) == 0x80);
        //成员变量Address的高位是0x8_，表示USB IN端点，IN表示主机接受USB功能设备发来的数据
        //高位是0x0_，表示USB OUT端点
        bool bIn = m_usbDevice->EndPoints[i]->bIn;
        bool bBulk = (m_usbDevice->EndPoints[i]->Attributes == 2);
        // Attributes表示端点的类型
        // 0 控制端点
        // 1 同步端点
        // 2 块端点
        // 3 中断端点
        // 对应USB的四种传输方式
        // 这里的例子分别指定为USB IN端点和USB OUT端点，均为块端点
        // 赋值
        if (bBulk && bIn)
            BulkInEpt = (CCyBulkEndPoint *) m_usbDevice->EndPoints[i];
        if (bBulk && !bIn)
            BulkOutEpt = (CCyBulkEndPoint *) m_usbDevice->EndPoints[i];
    }


    /*if (USBDevice->bHighSpeed)
    {
        // int i++;// Do something
    }*/

    /*
    /// Official Example: 块传输(异步传输)实例
    // This example assumes that the device automatically sends back,
    // over its bulk-IN endpoint, any bytes that were received over its
    // bulk-OUT endpoint (commonly referred to as a loopback function)
    CCyUSBDevice *USBDevice = new CCyUSBDevice(nullptr);
    OVERLAPPED outOvLap, inOvLap;
    outOvLap.hEvent = CreateEvent(nullptr, false, false, L"CYUSB_OUT");
    inOvLap.hEvent = CreateEvent(nullptr, false, false, L"CYUSB_IN");

    unsigned char inBuf[128];//数据缓存区
    ZeroMemory(inBuf, 128);  //申请内存，下同
    unsigned char buffer[128];
    LONG length = 128;

    // 初始化输入输出
    UCHAR *outContext = USBDevice->BulkOutEndPt->BeginDataXfer(buffer, length, &outOvLap);
    UCHAR *inContext = USBDevice->BulkInEndPt->BeginDataXfer(inBuf, length, &inOvLap);
    //BulkInEndPt 指向第一个块输入端点数据的指针
    //BulkOutEndPt 指向第一个块输出端点数据的指针

    // 等待数据介绍
    USBDevice->BulkOutEndPt->WaitForXfer(&outOvLap,100); //等待时长：100ms
    USBDevice->BulkInEndPt->WaitForXfer(&inOvLap,100);

    // 完成传输
    USBDevice->BulkOutEndPt->FinishDataXfer(buffer, length, &outOvLap, outContext);
    USBDevice->BulkInEndPt->FinishDataXfer(inBuf, length, &inOvLap, inContext);

    // 关闭输入输出
    CloseHandle(outOvLap.hEvent);
    CloseHandle(inOvLap.hEvent);
    */
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initRealtimePlot()
{
    ui->customplot->xAxis->setLabel("framenumber");
    ui->customplot->yAxis->setLabel("RGB");
    //设置y轴范围
    ui->customplot->yAxis->setRange(0, 255);

    ui->customplot->addGraph(); // red line
    ui->customplot->graph(0)->setPen(QPen(Qt::red));
    ui->customplot->addGraph(); // green line
    ui->customplot->graph(1)->setPen(QPen(Qt::green));
    ui->customplot->addGraph(); // blue line
    ui->customplot->graph(2)->setPen(QPen(Qt::blue));
}

void MainWindow::addData(double r, double g, double b, double x)
{
    plotSize++;
    if(plotSize == MAX_NUM)
    {
        xVector.pop_front();
        rValue.pop_front();
        gValue.pop_front();
        bValue.pop_front();
        plotSize--;
    }
    xVector.push_back(x);
    rValue.push_back(r);
    gValue.push_back(g);
    bValue.push_back(b);
}

void on_GaussianBlurTrackbar(int, void*)
{
    //根据输入的width和height重新计算ksize.width和ksize.height
    g_kernelWidthValue = g_nKwidthTrackBarValue * 2 + 1;
    g_kernelHeightValue = g_nKheightTrackBarValue * 2 + 1;

    GaussianBlur(srcImage, dstImage, Size(g_kernelWidthValue, g_kernelHeightValue),
                                   g_nsigmaXTrackBarValue, g_nsigmaYTrackBarValue);

    //imshow("GaussianBlur Image", dstImage);
}

QImage  Mat2QImage(const Mat &inputMat)
{
    Mat tmpMat;
    QImage qImg;
    if(inputMat.channels() == 1) //灰度图像
    {
        tmpMat = inputMat;
        qImg = QImage((const uchar*)tmpMat.data,   //指向所用图像数据块的指针
                    tmpMat.cols, tmpMat.rows,
                    tmpMat.cols*tmpMat.channels(), //bytesPerLine，每行的字节数
                    QImage::Format_Indexed8);      //利用8bits存储灰度值
    }
    else
    {   //Mat是BGR排列, QImage是RGB排列，利用cvtColor变换颜色空间
        cvtColor(inputMat, tmpMat, CV_BGR2RGB);
        qImg = QImage((const uchar*)tmpMat.data,   //指向所用图像数据块的指针
                    tmpMat.cols, tmpMat.rows,
                    tmpMat.cols*tmpMat.channels(), //bytesPerLine，每行的字节数
                    QImage::Format_RGB888);        //利用24bits真彩色存储RGB值
    }
    return qImg.copy();//deep copy
}

void MainWindow::nextFrame()
{
    Mat dst;
    int size;
    int posx, posy;
    QColor color;
    QString text = "";
    double r,g,b;           //摄取RGB值
    double s1=0, s2=0, s3=0;//对RGB值分别求和

    capture >> frame;
    if (!frame.empty())
    {
        //---framenumber更新
        framenumber++;
        ui->label_3->setText(tr("System is running!\nThe number of current frame is\n%1.").arg(framenumber, 5));

        //---label_2显示高斯模糊处理后的图像
        GaussianBlur(frame, dst, Size(5,5), 0, 0);
        image = Mat2QImage(dst);
        ui->label_2->setPixmap(QPixmap::fromImage(image));

        //---如果含有标记像素点，则更改frame，显示标记点及其RGB值
        //---否则，frame不变，显示原图像
        size = Points.size();
        if(size > 0)
        {
            image = Mat2QImage(frame);
            for(int i=0 ; i < size; i++)
            {
                //获取像素点坐标
                posx = Points[i].x();
                posy = Points[i].y();

                //提取像素RGB值
                color = image.pixel(Points[i]);
                r = color.red();
                g = color.green();
                b = color.blue();
                s1 += r;
                s2 += g;
                s3 += b;

                text += QString("\nX: %1 Y: %2   RGB: %3, %4, %5").arg(posx, 3).arg(posy, 3).arg(r, 3).arg(g, 3).arg(b, 3);
                //, 3表示占据的宽度

                //标记坐标点(红色)
                circle(frame, Point(posx, posy), 3, Scalar(0,0,255), -1);
                //3表示半径，Scalar(B,G,R)，-1表填充
            }
            //设置显示文本
            ui->label_16->setText(text);

            //添加图像数据点
            addData(s1/size, s2/size, s3/size, framenumber);
            ui->customplot->graph(0)->setData(xVector, rValue);
            ui->customplot->graph(1)->setData(xVector, gValue);
            ui->customplot->graph(2)->setData(xVector, bValue);
            //设置x轴范围
            ui->customplot->xAxis->setRange(framenumber-MAX_NUM*(1.05), framenumber+MAX_NUM*(0.05));
            ui->customplot->replot();
        }

        //---label显示frame
        image = Mat2QImage(frame);
        ui->label->setPixmap(QPixmap::fromImage(image));
    }
}

void MainWindow::readMyCom()//读串口函数
{
    QByteArray temp = myCom->readAll();    //读取串口缓冲区的所有数据给临时变量temp
    ui->textBrowser->insertPlainText(temp);//将串口的数据显示在窗口的文本浏览器中
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->label)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            if(Points.size() < pointNumber && capture.isOpened())
            {
                //转化为鼠标事件
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                //将事件的全局坐标转化成相对与label部件的坐标
                QPoint temp = ui->label->mapFromGlobal(mouseEvent->globalPos());
                //qDebug()<<"x:"<<temp.x()<<"y:"<<temp.y()<<endl;
                Points.append(temp);
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return QMainWindow::eventFilter(watched,event);
}

/////////////////////slot
void MainWindow::on_pushButton_1_clicked()
{
    if (capture.isOpened())   //关闭开启的捕像设备
        capture.release();
    QString filename =QFileDialog::getOpenFileName(this,tr("Open Video File"),".",tr("Video Files(*.avi *.mp4 *.flv *.mkv)"));
    capture.open(filename.toLocal8Bit().data());  //打开已有视频文件
    if (capture.isOpened())
    {
        rate = capture.get(CAP_PROP_FPS);//Frame rate帧率 FPS每秒多少帧
        //rate = capture.get(5);//VideoCapture属性编号0 1 2 3 4... FPS对应编号为5
        capture >> frame;//frame：Mat类型
        if (!frame.empty())
        {
            framenumber = 1;
            image = Mat2QImage(frame); //函数定义见上方
            ui->label->setPixmap(QPixmap::fromImage(image));//实际的彩色图像
            timer = new QTimer(this);           //设置定时器
            timer->setInterval(int(1000/rate));     //设置与FPS相匹配的时间间隔(毫秒)
            connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));//nextFrame()定义见上部
            timer->start();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (capture.isOpened()) //解读同on_pushButton_1_clicked()
        capture.release();

    //设置相机参数
    //capture.set(CAP_PROP_FRAME_WIDTH, 1920); //宽度
    //capture.set(CAP_PROP_FRAME_HEIGHT, 1200);//高度
    //capture.set(CAP_PROP_FRAME_COUNT, 160);  //帧数
    //打开默认相机
    capture.open(0);
    //图像参数设置
    //capture.set(CAP_PROP_BRIGHTNESS, 1); //亮度
    //capture.set(CAP_PROP_CONTRAST,40);   //对比度 40
    //capture.set(CAP_PROP_SATURATION, 50);//饱和度 50
    //capture.set(CAP_PROP_HUE, 50);       //色调   50
    //capture.set(CAP_PROP_EXPOSURE, 50);  //曝光   50

    if (capture.isOpened())
    {
        rate = capture.get(CAP_PROP_FPS);
        //rate = capture.get(5);
        capture >> frame;
        if (!frame.empty())
        {
            framenumber = 1;
            image = Mat2QImage(frame);
            ui->label->setPixmap(QPixmap::fromImage(image));
            timer = new QTimer(this);
            timer->setInterval(int(1000/rate));
            connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
            timer->start();
        }
    }
    else
    {
        QMessageBox::warning(nullptr, "warning", "camera is not open!", QMessageBox::Yes, QMessageBox::Yes);
        //Mat cannyImg ;
        //Canny(frame, cannyImg, 0, 30, 3);
        //namedWindow("Canny");
        //imshow("camera is not open!", cannyImg);
    }
}
void MainWindow::on_pushButton_3_clicked()
{
    Mat cannyImg;
    Canny(frame, cannyImg, 0, 30, 3);//(src, dst, threshold1, threshold2, aperture size for the Sobel operator)
    namedWindow("Canny");            //创建窗口并显示图像
    imshow("Canny", cannyImg);
}

void MainWindow::on_pushButton_4_clicked()
{
    Mat src;
    if (!capture.isOpened())
        qDebug()<<"ERROR! Unable to open camera.\n";
    capture >> src;
    if (src.empty())
        qDebug()<<"ERROR! Blank frame grabbed.\n";

    //---初始化VideoWriter
    int codec = VideoWriter::fourcc('P','I','M','1');
    //视频播放软件通过查询FourCC代码并且寻找与FourCC代码相关联的视频解码器来播放特定的视频流
    double fps = capture.get(CAP_PROP_FPS);
    bool isColor = (src.type() == CV_8UC3);//检查是否为彩色图像
    writer.open("./myRecord.avi", codec, fps, src.size(), isColor);
    if (!writer.isOpened())
        qDebug()<<"Could not open the output video file for write.\n";

    //---录制一个视频
    int t=20;            //时长(seconds)
    int num=int(t*fps);  //帧数，帧数越多，视频也就越长
    while(num--)
    {
        if (!capture.read(src))//读取新的图像
        {
            qDebug()<<"ERROR! Blank frame grabbed.\n";
            break;
        }
        writer.write(src);    //并写入文件
    }

    //---保存视频之后，取消该键，按钮变为灰色
    ui->pushButton_4->setDisabled(true);
}

void MainWindow::on_pushButton_5_clicked()
{
    writer.release();
}
/////////////////////////com  读串口函数

void MainWindow::on_openButton_clicked()
{}

void MainWindow::on_closeButton_clicked(){}

void MainWindow::on_sendMsgButton_clicked(){}

void MainWindow::on_action_3_triggered()
{
    dialog = new inputDialog;

    dialog->setModal(true);
    dialog->show();
}

void MainWindow::on_action_4_triggered()
{
    mddialog = new modifyDialog;

    mddialog->setModal(true);
    mddialog->show();

}
