#include "styledialog.h"
#include "ui_styledialog.h"

StyleDialog::StyleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StyleDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, [=](){
        styleName = ui->comboBox->currentText();
        QDialog::close();
    });
}

StyleDialog::~StyleDialog()
{
    delete ui;
}

QString StyleDialog::getStyleName()
{
    return styleName;
}
