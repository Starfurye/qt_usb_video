#ifndef MODIFYDIALOG_H
#define MODIFYDIALOG_H

#include <QDialog>
#include "updatedialog.h"

namespace Ui {
class modifyDialog;
}

class modifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit modifyDialog(QWidget *parent = nullptr);
    ~modifyDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

private:
    Ui::modifyDialog *ui;
    updateDialog *upddialog;
};

#endif // MODIFYDIALOG_H
