#include "findfiledialog.h"
#include "ui_findfiledialog.h"

void FindFileDialog::setButtonSearchHelperRemove()
{
    int count = markdownTopItem->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem * subItem = markdownTopItem->child(0);
        delete subItem;
    }

    count = latexTopItem->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem * subItem = latexTopItem->child(0);
        delete subItem;
    }

    count = wordTopItem->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem * subItem = wordTopItem->child(0);
        delete subItem;
    }
}

void FindFileDialog::setButtonSearchHelperAdd(QString currentDirPath, QStringList filters)
{
    QDir dir(currentDirPath);
    QDir rootDir(dirPath);

    foreach (QString fileName, dir.entryList(filters, QDir::Files, QDir::Time)) {

        QFileInfo info(currentDirPath + '/' + fileName);

        QTreeWidgetItem * subItem =
                new QTreeWidgetItem(
                    QStringList()
                    << fileName
                    << rootDir.relativeFilePath(currentDirPath + '/' + fileName)
                    << tr("%1 bytes").arg(info.size())
                    << info.lastModified().toString("yyyy-MM-dd hh:mm:ss")
                    );

        QString suffix = info.suffix();

        if (suffix == "md") {
            markdownTopItem->addChild(subItem);
        } else if (suffix == "tex") {
            latexTopItem->addChild(subItem);
        } else if (suffix == "docx") {
            wordTopItem->addChild(subItem);
        } else {
            delete subItem;
        }
    }

    foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Time)) {
        setButtonSearchHelperAdd(currentDirPath + '/' + subdir, filters);
    }
}

void FindFileDialog::setButtonSearch()
{
    connect(ui->lineEdit, &QLineEdit::textChanged, [=](){
        setButtonSearchHelperRemove();

        // ingore suffix
        // do not confuse with regular expression
        QString fileNameRegExp = ui->lineEdit->text() + ".*";
        setButtonSearchHelperAdd(dirPath, QStringList() << fileNameRegExp);

        ui->treeWidget->expandAll();
    });

//    connect(ui->buttonSearch, &QPushButton::clicked, [=](){
//        setButtonSearchHelperRemove();

//        // ingore suffix
//        // do not confuse with regular expression
//        QString fileNameRegExp = ui->lineEdit->text() + ".*";
//        setButtonSearchHelperAdd(dirPath, QStringList() << fileNameRegExp);

//        ui->treeWidget->expandAll();
//    });
}

void FindFileDialog::setButtonLoad()
{
    connect(ui->buttonLoad, &QPushButton::clicked, [=](){
        auto target = ui->treeWidget->currentItem();
        if (target == nullptr) return;

        QString path = target->text(1);
        if (path != "") {
            filePath = dirPath + "\\" + path; // relative to absolute
            this->done(0); // set successfully
        } else {
            return;
        }
    });
}

void FindFileDialog::setBasicUI()
{
    ui->lineEdit->setFont(QFont("Cascadia Code"));
    ui->lineEdit->setText("Search \"" + dirPath + "\"");

    ui->treeWidget->setHeaderLabels(QStringList() << "Type (name)" << "Path" << "Size" << "(M)Time");
    ui->treeWidget->header()->setDefaultSectionSize(300);

    markdownTopItem = new QTreeWidgetItem(QStringList() << "Markdown");
    latexTopItem = new QTreeWidgetItem(QStringList() << "Latex");
    wordTopItem = new QTreeWidgetItem(QStringList() << "Word");

    ui->treeWidget->addTopLevelItem(markdownTopItem);
    ui->treeWidget->addTopLevelItem(wordTopItem);
    ui->treeWidget->addTopLevelItem(latexTopItem);

//    QTreeWidgetItem * temp =
//            new QTreeWidgetItem(
//                QStringList() << "demo.md"
//                << "D:\\Users\\VGalaxy\\Desktop\\document_preview_tests\\Markdown\\demo.md");
//    markdownTopItem->addChild(temp);
}

FindFileDialog::FindFileDialog(QString dirPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindFileDialog)
{
    ui->setupUi(this);

    this->dirPath = dirPath;

    setBasicUI();
    setButtonLoad();
    setButtonSearch();

}

FindFileDialog::~FindFileDialog()
{
    delete ui;
}

QString FindFileDialog::getFilePath()
{
    return filePath;
}
