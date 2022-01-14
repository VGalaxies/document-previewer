#ifndef STYLEDIALOG_H
#define STYLEDIALOG_H

#include <QDialog>

namespace Ui {
class StyleDialog;
}

class StyleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StyleDialog(QWidget *parent = nullptr);
    QString getStyleName();
    ~StyleDialog();

private:
    QString styleName;
    Ui::StyleDialog *ui;
};

#endif // STYLEDIALOG_H
