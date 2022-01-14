#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QFile>

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QWidget *parent = nullptr);
    ~EditDialog();

private:
    Ui::EditDialog *ui;
};

#endif // EDITDIALOG_H
