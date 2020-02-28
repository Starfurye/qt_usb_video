#ifndef GETALLDIALOG_H
#define GETALLDIALOG_H

#include <QDialog>

namespace Ui {
class getallDialog;
}

class getallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit getallDialog(QWidget *parent = nullptr);
    ~getallDialog();

private:
    Ui::getallDialog *ui;
};

#endif // GETALLDIALOG_H
