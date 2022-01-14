#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(400, 300);

    setButtonLoadFromFile();
    setButtonLoadFromDir();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::handleMarkdown()
{
    // 1. load
    QFile inFile(oriFilePath);
    inFile.open(QIODevice::ReadOnly);

    // 2. edit
    QDialog dialog;
    QPlainTextEdit textedit(&dialog);
    dialog.setFixedSize(1600, 900);
    textedit.setFixedSize(1600, 900);
    textedit.setPlainText(inFile.readAll());
    dialog.exec();
    inFile.close();

    // 3. parse
    MarkdownScanner scanner(textedit.toPlainText().toStdString().append("\n")); // 加上一个换行符
    scanner.scanTokens();

    MarkdownParser parser(scanner.getTokens());
    parser.start();
    std::string res = parser.getRes();

    // 4. generate
    htmlFilePath = oriFilePath.replace(".md", "_md").append(".html");
    QFile outFile(htmlFilePath);
    outFile.open(QIODevice::WriteOnly);
    outFile.write(res.c_str());
    outFile.close();

    return true;
}

bool MainWindow::handleWord()
{
    oriFilePath.replace(QString("/"), QString("\\"));
    // Otherwise
    // QAxBase: Error calling IDispatch member SaveAs2: Exception thrown by server

    // 1. load
    QAxWidget * word = new QAxWidget("Word.Application"); // 打开 Word
    word->setProperty("Visible", false); // 设置程序不显示
    QAxObject * doc = word->querySubObject("Documents");
    // 通过Document打开要获取文件内容的文件
    doc->dynamicCall("Open(const QVariant&)", QVariant(oriFilePath));
    QAxObject * ori = word->querySubObject("ActiveDocument");

    QVariant xmlFilePath(oriFilePath.replace(".docx", ".xml")); // 保存的文档的名称
    QVariant xmlFormat(19);
    // 文档的保存格式为 Open XML
    // PS: Filtered HTML -> 10

    ori->dynamicCall("SaveAs2(const QVariant&, const QVariant&)", xmlFilePath, xmlFormat);
    delete ori;
    doc->dynamicCall("Close(boolean)", true);
    delete doc;
    word->dynamicCall("Quit(void)"); // 退出 Word
    word->close();
    delete word;

    // 2. parse
    WordXML xml(xmlFilePath.toString());
    xml.start();
    WordDocument document = xml.getDocument();
    WordParser parser(document);
    parser.start();

    // QFile::remove(xmlFilePath.toString());

    // 3. edit
    QDialog dialog;
    QTextEdit textedit(&dialog);
    dialog.setFixedSize(1600, 900);
    textedit.setFixedSize(1600, 900);
    textedit.setText(parser.getRes().toStdString().c_str());
    dialog.exec();

    // 4. generate
    htmlFilePath = oriFilePath.replace(".xml", "_docx").append(".html");
    QFile file(htmlFilePath);
    file.open(QIODevice::WriteOnly);
    // modify the head
    // reason: keep the format
    QString oriRes = parser.getRes();
    int st = oriRes.indexOf("<head>");
    int ed = oriRes.indexOf("</head>");
    QString oriHead = oriRes.mid(st, ed - st + 7);
    QString res = textedit.toHtml();
    res.replace(QRegExp("<head>.*</head>"), oriHead);
    file.write(res.toStdString().c_str());
    file.close();

    return true;
}

bool MainWindow::handleLatex()
{
    // 1. load and parse
    TexParser parser(oriFilePath);
    bool state = parser.start();
    if(!state){
        QMessageBox::critical(this, "Error", "Failed to call pandoc.");
        return false;
    }

    // 2. edit
    // using EditDialog class
//    EditWidget * editWindow = new EditWidget(this);
//    QTextEdit * textedit = editWindow->findChild<QTextEdit *>("textEdit");
//    textedit->setText(parser.getRes().toStdString().c_str());
//    editWindow->setWindowFlags(editWindow->windowFlags()|Qt::Dialog);
//    editWindow->setWindowModality(Qt::ApplicationModal);
//    editWindow->show();
    EditDialog * editDialog = new EditDialog(this);
    QTextEdit * textedit = editDialog->findChild<QTextEdit *>("textEdit");
    textedit->setText(parser.getRes().toStdString().c_str());
    editDialog->exec();

    // 3. generate
    htmlFilePath = oriFilePath.replace(".tex", "_tex").append(".html");
    QFile file(htmlFilePath);
    file.open(QIODevice::WriteOnly);
    // modify the head
    // reason: UTF-8 support
    QString res = textedit->toHtml();
    res.insert(res.indexOf("<html><head><meta ") + 18, "charset=\"UTF-8\" ");
    // choose style
    StyleDialog * styleDialog = new StyleDialog(this);
    styleDialog->exec();
    QString styleName = styleDialog->getStyleName();
    //qDebug() << styleName;
    if(styleName != "default"){
        QString cssFileName = ":/style/css/" + styleName + ".css";
        QFile cssFile(cssFileName);
        cssFile.open(QFileDevice::ReadOnly);
        QString css =  cssFile.readAll().toStdString().c_str();
        res.replace("\np, li { white-space: pre-wrap; }\n",
                    css);
        cssFile.close();
    }
    file.write(res.toStdString().c_str());
    file.close();

    return true;
}

void MainWindow::display()
{
    // display
    assert(htmlFilePath != "");
    QAxWidget * axWidget = new QAxWidget;
    axWidget->setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}")); // 注册组件ID
    axWidget->setProperty("DisplayAlerts", false); // 不显示警告信息
    axWidget->setProperty("DisplayScrollBars", true); // 显示滚动条

//        QString webstr=QString("https://cn.bing.com/"); // 设置要打开的网页
//        ui->axWidget->dynamicCall("Navigate(const QString&)", webstr); // 显示网页

    axWidget->dynamicCall("Navigate(const QString&)", htmlFilePath.trimmed());

    // axWidget->setParent(this); // 对象树

    QRect screenRect = QGuiApplication::screens().at(0)->availableGeometry();

    axWidget->setGeometry(screenRect);
    axWidget->show();
}

void MainWindow::handleAndDisplay()
{
    QFileInfo info(oriFilePath);

    bool state = false;
    QString suffix = info.suffix();
    if (suffix == "md") {
        state = handleMarkdown();
    } else if (suffix == "docx") {
        state = handleWord();
    } else if (suffix == "tex"){
        state = handleLatex();
    } else {
        if (oriFilePath != "")
            QMessageBox::information(this, "Info", "Only support markdown, docx and tex file.");
        state = false;
    }

    if (state) {
        // QMessageBox::information(this, "Info", "Success.");
        display();
    }

    reset();
}

void MainWindow::reset()
{
    // reset
    oriFilePath = "";
    htmlFilePath = "";
}

void MainWindow::setButtonLoadFromFile()
{
    connect(ui->loadFromFile, &QPushButton::clicked, [=](){
        oriFilePath = QFileDialog::getOpenFileName(
                    this, "Open from file", "D:/Users/VGalaxy/Desktop"); // 不通用
        handleAndDisplay();
    });
}

void MainWindow::setButtonLoadFromDir()
{
    connect(ui->loadFromDir, &QPushButton::clicked, [=](){
        // 获取文件夹路径
        QString dirPath = QFileDialog::getExistingDirectory(
                   this, "Open from directory", "D:/Users/VGalaxy/Desktop");

        if (dirPath.isEmpty()) {
            return;
        }

        FindFileDialog * findFileDialog = new FindFileDialog(dirPath, this);
        findFileDialog->exec();

        oriFilePath = findFileDialog->getFilePath();
        if (oriFilePath != "") {
            handleAndDisplay();
        }
    });
}
