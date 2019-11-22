#include "modifydialog.h"
#include "ui_modifydialog.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>

static QStandardItemModel *model = new QStandardItemModel();
static int curRow;
QString curId;
static QString id[30]; // 用于记录符合条件患者id

modifyDialog::modifyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modifyDialog)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("查看患者信息"));

    ui->tableView->setModel(model);
    model->setColumnCount(4);
    model->setHeaderData(0,Qt::Horizontal, QStringLiteral("姓名"));
    model->setHeaderData(1,Qt::Horizontal, QStringLiteral("性别"));
    model->setHeaderData(2,Qt::Horizontal, QStringLiteral("年龄"));
    model->setHeaderData(3,Qt::Horizontal, QStringLiteral("身份证号"));
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

modifyDialog::~modifyDialog()
{
    delete ui;
}

void modifyDialog::on_pushButton_clicked()
{
    // query button
    QSqlQuery sql_query;
    QString Queryname = ui->textEdit->toPlainText();
    QString select_sql = "select * from qtusb where name = '" + Queryname + "'";

    int counts = 0;

    sql_query.prepare(select_sql);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        while(sql_query.next())
        {
            QString name = sql_query.value(0).toString();
            bool sex = sql_query.value(1).toBool();
            QString age = sql_query.value(2).toString();
            id[counts] = sql_query.value(3).toString();

            model->setItem(counts,0,new QStandardItem(name));
            if(sex)
            {
                QString _ = QStringLiteral("女");
                model->setItem(counts,1,new QStandardItem(_));
            }
            else {
                QString _ = QStringLiteral("男");
                model->setItem(counts,1,new QStandardItem(_));
            }
            model->setItem(counts,2,new QStandardItem(age));
            model->setItem(counts,3,new QStandardItem(id[counts]));

            ++counts;
        }
        if(!counts)
        {
            QMessageBox::about(nullptr, QStringLiteral("查询结果"), QStringLiteral("未找到该条记录！"));
        }
    }
}

void modifyDialog::on_pushButton_2_clicked()
{
    //delete button
    QSqlQuery sql_query;
    QString delete_sql = "delete from qtusb where id = '" + curId + "'";
    sql_query.prepare(delete_sql);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
    else {
        QMessageBox::about(nullptr, QStringLiteral("提示"), QStringLiteral("删除成功"));
    }

    model->removeRows(0,model->rowCount());
}

void modifyDialog::on_tableView_clicked(const QModelIndex &index)
{
    curRow = ui->tableView->currentIndex().row();
    curId = id[curRow];
}


void modifyDialog::on_pushButton_3_clicked()
{
    // update button
    upddialog = new updateDialog;

    upddialog->setModal(true);
    upddialog->show();
    model->removeRows(0,model->rowCount());
}
