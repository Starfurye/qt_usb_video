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

static QVector<QString> id;

getallDialog::getallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::getallDialog)
{
    ui->setupUi(this);
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
