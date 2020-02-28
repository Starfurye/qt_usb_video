#include "inputdialog.h"
#include "ui_inputdialog.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

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
    if (addName == "") {
        QMessageBox::critical(nullptr, QStringLiteral("错误"), QStringLiteral("请输入患者姓名"), QMessageBox::Ok);
        return;
    }
    sql_query.addBindValue(addName);           // insert name
    sql_query.addBindValue(addSex);             // insert sex, male: 0, female: 1
    addAge = ui->spinBox->text();
    if (addAge == "") {
        QMessageBox::critical(nullptr, QStringLiteral("错误"), QStringLiteral("请输入患者年龄"), QMessageBox::Ok);
        return;
    }
    sql_query.addBindValue(addAge.toInt());      // insert age
    addID = ui->textEdit_3->toPlainText();
    if (addID == "" || addID.size() != 18) {
        QMessageBox::critical(nullptr, QStringLiteral("错误"), QStringLiteral("请输入正确的18位身份证号"), QMessageBox::Ok);
        return;
    }
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
