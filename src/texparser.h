#ifndef TEXPARSER_H
#define TEXPARSER_H

#include <QString>
#include <QProcess>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <iostream>
#include <string>
#include <QRegExp>
#include <QDateTime>

class TexParser
{
public:
    TexParser(QString);
    bool start();
    QString getRes();
private:
    void html();
    void body();
    void head();
    void style();
    void meta();
    void preamble();
    void pictureLinkFix();

    QString path;
    QString res;
    QString bodyContent;


};

#endif // TEXPARSER_H
