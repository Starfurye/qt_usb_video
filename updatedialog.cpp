#include "updatedialog.h"
#include "ui_updatedialog.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

#include"modifydialog.h"

extern QString curId;

updateDialog::updateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updateDialog)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("更新患者信息"));
    ui->buttonGroup->setId(ui->radioButton, 0);
    ui->buttonGroup->setId(ui->radioButton_2, 1);
    ui->radioButton->setChecked(true);
}

updateDialog::~updateDialog()
{
    delete ui;
}

void updateDialog::on_pushButton_2_clicked()
{
    // cancel button
    emit sendsignal();
    this->close();
}

void updateDialog::on_pushButton_clicked()
{
    // Apply button
    QSqlQuery sql_query;
    int btnId = ui->buttonGroup->checkedId();

    QString update_sql = "update qtusb set name = :name, sex = :sex, age = :age, id = :newid where id = :id";
    sql_query.prepare(update_sql);
    sql_query.bindValue(":name", ui->textEdit->toPlainText());
    sql_query.bindValue(":sex", btnId);
    sql_query.bindValue(":age", ui->textEdit_2->toPlainText().toInt());
    sql_query.bindValue(":newid", ui->textEdit_3->toPlainText());
    sql_query.bindValue(":id",curId);

    if(!sql_query.exec())
    {
        qDebug() << sql_query.lastError();
    }
    else
    {
        QMessageBox::about(nullptr,QStringLiteral("提示"),QStringLiteral("更新成功"));
    }

    emit sendsignal();
    this->close();
}
