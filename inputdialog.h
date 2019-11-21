#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>

namespace Ui {
class inputDialog;
}

class inputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit inputDialog(QWidget *parent = nullptr);
    ~inputDialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

signals:
    void sendsignal();

private:
    Ui::inputDialog *ui;
};

#endif // INPUTDIALOG_H
