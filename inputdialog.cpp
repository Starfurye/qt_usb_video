#include "inputdialog.h"
#include "ui_inputdialog.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

static QString addName,addAge,addID;
static int addSex;

inputDialog::inputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inputDialog)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("输入患者信息"));
    ui->buttonGroup->setId(ui->radioButton, 0);
    ui->buttonGroup->setId(ui->radioButton_2, 1);
    ui->radioButton->setChecked(true);
}

inputDialog::~inputDialog()
{
    delete ui;
}

void inputDialog::on_pushButton_2_clicked()
{
    // cancel button
    emit sendsignal();
    this->close();
}

void inputDialog::on_pushButton_clicked()
{
    QSqlQuery sql_query;
    if(!sql_query.exec("create table qtusb(name text, sex numeric, age int, id text primary key)"))
        {
            qDebug() << "Error: Fail to create table."<< sql_query.lastError();
        }
    else
        {
            qDebug() << "Table created!";
        }

    addSex = ui->buttonGroup->checkedId();

    // insert data
    QString insert_sql = "insert into qtusb values (?, ?, ?, ?)";
    sql_query.prepare(insert_sql);
    addName = ui->textEdit->toPlainText();
    sql_query.addBindValue(addName);           // insert name
    sql_query.addBindValue(addSex);             // insert sex, male: 0, female: 1
    addAge = ui->spinBox->text();
    sql_query.addBindValue(addAge.toInt());      // insert age
    addID = ui->textEdit_3->toPlainText();
    sql_query.addBindValue(addID);              // insert id

    if(!sql_query.exec())
    {
        qDebug() << sql_query.lastError();
    }
    else
    {
        qDebug() << "inserted!";
    }

    // ok button
    emit sendsignal();
    this->close();
}
