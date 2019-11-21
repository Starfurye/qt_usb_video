#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class updateDialog;
}

class updateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit updateDialog(QWidget *parent = nullptr);
    ~updateDialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

signals:
    void sendsignal();

private:
    Ui::updateDialog *ui;
};

#endif // UPDATEDIALOG_H
