#include "texparser.h"

TexParser::TexParser(QString path)
{
    this->path = path;
}

bool TexParser::start()
{
    QProcess process;
    QString tmp = path;
    QString htmlPath = tmp.replace(".tex", "_tex").append(".html");
    process.start("pandoc", QStringList() << path << "-o" << htmlPath);
    //qDebug() << process.arguments();
    process.waitForFinished();

    QFile file(htmlPath);

    // assume: fail to call pandoc
    if(!file.exists())
        return false;

    file.open(QFile::ReadOnly);
    bodyContent = file.readAll();
    file.close();
    QFile::remove(htmlPath);

    html();
    return true;
}

void TexParser::pictureLinkFix()
{
//    QRegExp regExpPic("<img src=\"(.*)\"");
//    regExpPic.setMinimal(true);

//    int pos = 0;
//    while ((pos = regExpPic.indexIn(res, pos)) != -1) {
//        QString picLink = regExpPic.cap(1);
//        qDebug() << picLink;
//        res.replace(picLink, picLink + ".jpg"); // assume jpg picture
//        pos += regExpPic.matchedLength() + 4;
//    }
}

void TexParser::html()
{
    res.append("<html>");
    head();
    body();
    res.append("</html>");
    pictureLinkFix();
}

void TexParser::meta()
{
    res.append("<meta charset=\"UTF-8\">");
}

void TexParser::head()
{
    res.append("<head>");
    meta();
    style();
    res.append("</head>");
}

void TexParser::style()
{

}

void TexParser::preamble()
{
    // preamble information
    QString title;
    QString author;
    QString date;

    QFile file(path);
    file.open(QFileDevice::ReadOnly);
    QString content = file.readAll().toStdString().c_str();

    QRegExp regExpTitle("title\\{(.*)\\}");
    QRegExp regExpAuthor("author\\{(.*)\\}");
    QRegExp regExpDate("date\\{(.*)\\}");
    regExpTitle.setMinimal(true); // greedy
    regExpAuthor.setMinimal(true); // greedy
    regExpDate.setMinimal(true); // greedy

    int pos = regExpTitle.indexIn(content);
    if(pos > -1)title = regExpTitle.cap(1);
    pos = regExpAuthor.indexIn(content);
    if(pos > -1)author = regExpAuthor.cap(1);
    pos = regExpDate.indexIn(content);
    if(pos > -1)date = regExpDate.cap(1);
    if(date == "\\today"){
        date = QDateTime::currentDateTime().toString("yyyy.MM.dd");
    }else if(date == ""){

    }else{
        assert(0);
    }

    // set format manually
    res.append("<p style=\"text-align: center; "
               "font-size: 60px; font-weight: 600; font-family: STZhongsong, SimSun; "
               "margin-top: 30px; margin-bottom: 30px\">"
               + title + "</p>");
    res.append("<p style=\"text-align: center; "
               "font-size: 18px; font-weight: 100; font-family: STXinwei, SimSun;\">"
               + author + "</p>");
    res.append("<p style=\"text-align: center; "
               "font-size: 18px; font-weight: 100; font-family: STXinwei, SimSun;\">"
               + date + "</p>");
}

void TexParser::body()
{
    preamble();
    res.append(bodyContent);
}

QString TexParser::getRes()
{
    return res;
}
