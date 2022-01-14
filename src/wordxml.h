#ifndef WORDXML_H
#define WORDXML_H

#include "worddocument.h"

#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QDomDocument>
#include <QDomNodeList>
#include <QString>
#include <QFile>
#include <QDebug>
#include <set>
#include <cassert>



class WordXML
{
public:
    WordXML(QString);
    void start();
    WordDocument getDocument() const;

private:
    void readProcess(QXmlStreamReader *);

    void domPart(QDomDocument *);

    void domRelationshipInfo(QDomNodeList *);

    void domStyleInfo(QDomNodeList *);
    void domStyleDefault(QDomNodeList *);
    void domStyleID(QDomElement *);

    void domBodyInfo(QDomNodeList *);

    shared_ptr<TableCol> domTableColInfo(QDomNodeList *);
    shared_ptr<TableRow> domTableRowInfo(QDomNodeList *);
    shared_ptr<Table> domTableInfo(QDomNodeList *);

    shared_ptr<SectionProperty> domSectionProperty(QDomNodeList *);
    shared_ptr<ParagraphProperty> domParagraphProperty(QDomNodeList *);
    shared_ptr<RunProperty> domRunProperty(QDomNodeList *);

    shared_ptr<ParagraphInfo> domParagraphInfo(QDomNodeList *);
    shared_ptr<RunInfo> domRunInfo(QDomNodeList *);

    QString path;
    WordDocument document;

    QString hyperlinkID = ""; // temporary storage of hyperlink information
};

#endif // WORDXML_H
