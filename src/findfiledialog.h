#ifndef FINDFILEDIALOG_H
#define FINDFILEDIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QDir>
#include <QDateTime>

namespace Ui {
class FindFileDialog;
}

class FindFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindFileDialog(QString, QWidget *parent = nullptr);
    ~FindFileDialog();
    QString getFilePath();

private:
    void setBasicUI();
    void setButtonLoad();
    void setButtonSearch();
    void setButtonSearchHelperAdd(QString currentDirPath, QStringList filters);
    void setButtonSearchHelperRemove();

    Ui::FindFileDialog *ui;
    QString dirPath;
    QString filePath;

    QTreeWidgetItem * markdownTopItem;
    QTreeWidgetItem * latexTopItem;
    QTreeWidgetItem * wordTopItem;
};

#endif // FINDFILEDIALOG_H
