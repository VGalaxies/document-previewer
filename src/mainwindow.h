#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mdparser.h"
#include "mdscanner.h"
#include "wordxml.h"
#include "wordparser.h"
#include "texparser.h"

#include "editdialog.h"
#include "styledialog.h"
#include "findfiledialog.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QAxWidget>
#include <QAxObject>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QScreen>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;

    void setButtonLoadFromFile();
    void setButtonLoadFromDir();
    void handleAndDisplay();

    void handleStyle();

    bool handleMarkdown();
    bool handleWord();
    bool handleLatex();

    void display();
    void reset();

    QString oriFilePath;
    QString htmlFilePath;
};
#endif // MAINWINDOW_H
