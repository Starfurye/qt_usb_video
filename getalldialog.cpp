#include "getalldialog.h"
#include "ui_getalldialog.h"
#include <QtDebug>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVector>
#include <QFileDialog>
#include <QDesktopServices>
#include <ActiveQt/QAxObject>
#include <QMessageBox>

static QVector<QString> id;

getallDialog::getallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::getallDialog)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("全部患者信息"));

    QStandardItemModel *model = new QStandardItemModel;
    model->setHorizontalHeaderLabels({QStringLiteral("身份证号"), QStringLiteral("性别"), QStringLiteral("年龄"), QStringLiteral("姓名")});
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);      // 自适应所有列，布满空间
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSqlQuery sql_query;
    QString select_sql = "select * from qtusb order by id asc";
    sql_query.prepare(select_sql);

    int counts = 0;
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
    else {
        while(sql_query.next())
        {
            QString name = sql_query.value(0).toString();
            bool sex = sql_query.value(1).toBool();
            QString age = sql_query.value(2).toString();
            id.append(sql_query.value(3).toString());

            model->setItem(counts, 0, new QStandardItem(id[counts]));
            if(sex)
            {
                QString _ = QStringLiteral("女");
                model->setItem(counts, 1, new QStandardItem(_));
            }
            else {
                QString _ = QStringLiteral("男");
                model->setItem(counts, 1, new QStandardItem(_));
            }
            model->setItem(counts, 2, new QStandardItem(age));
            model->setItem(counts, 3, new QStandardItem(name));

            ++counts;
        }
    }

    ui->tableView->setModel(model);
    ui->tableView->show();
}

getallDialog::~getallDialog()
{
    delete ui;
}

void getallDialog::on_pushButton_released()
{
    QString title = QStringLiteral("全部患者信息");
    QString fileName = QFileDialog::getSaveFileName(ui->tableView, QStringLiteral("保存"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),QStringLiteral("Excel 文件(*.xls *.xlsx)"));
    if (fileName != "") {
        QAxObject *excel = new QAxObject;
        if (excel->setControl("Excel.Application")) //连接Excel控件
        {
            excel->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
            excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
            workbooks->dynamicCall("Add");//新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);

            int i,j;
            //QTableView 获取列数
            int colcount=ui->tableView->model()->columnCount();
            //QTableView 获取行数
            int rowcount=ui->tableView->model()->rowCount();

            QAxObject *cell,*col;
            //标题行
            cell=worksheet->querySubObject("Cells(int,int)", 1, 1);
            cell->dynamicCall("SetValue(const QString&)", title);
            cell->querySubObject("Font")->setProperty("Size", 18);
            //调整行高
            worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 30);
            //合并标题行
            QString cellTitle;
            cellTitle.append("A1:");
            cellTitle.append(QChar(colcount - 1 + 'A'));
            cellTitle.append(QString::number(1));
            QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
            range->setProperty("WrapText", true);
            range->setProperty("MergeCells", true);
            range->setProperty("HorizontalAlignment", -4108);//xlCenter
            range->setProperty("VerticalAlignment", -4108);//xlCenter

            //列标题
            for (i = 0; i < colcount; ++i) {
                QString columnName;
                columnName.append(QChar(i + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", ui->tableView->columnWidth(i)/6);
                cell=worksheet->querySubObject("Cells(int,int)", 2, i+1);

                //QTableView 获取表格头部文字信息
                columnName=ui->tableView->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            //QTableView 获取表格数据部分
            for(i=0;i<rowcount;i++) //行数
            {
                for (j=0;j<colcount;j++)   //列数
                {
                    QModelIndex index = ui->tableView->model()->index(i, j);
                    QString strdata=ui->tableView->model()->data(index).toString();
                    worksheet->querySubObject("Cells(int,int)", i+3, j+1)->dynamicCall("SetValue(const QString&)", strdata);
                }
            }

            //画框线
            QString lrange;
            lrange.append("A2:");
            lrange.append(colcount - 1 + 'A');
            lrange.append(QString::number(ui->tableView->model()->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", lrange);
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
            //调整数据区行高
            QString rowsName;
            rowsName.append("2:");
            rowsName.append(QString::number(ui->tableView->model()->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", rowsName);
            range->setProperty("RowHeight", 20);
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));//保存至fileName
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel = nullptr;
            if (QMessageBox::question(nullptr,QStringLiteral("完成"),QStringLiteral("文件已经导出，是否现在打开？"),QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
        else
        {
            QMessageBox::warning(nullptr,QStringLiteral("错误"),QStringLiteral("未能创建 Excel 对象，请安装 Microsoft Excel。"),QMessageBox::Apply);
        }
    }
}
