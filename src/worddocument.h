#ifndef WORDDOCUMENT_H
#define WORDDOCUMENT_H

#include <QString>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <variant>
#include <QDebug>

using std::map;
using std::vector;
using std::set;

using std::shared_ptr;
using std::make_pair;
using std::make_shared;

using std::variant;
using std::holds_alternative;
using std::get;

class RunProperty
{
    friend class WordParser;

public:
    RunProperty(QString, QString, QString, QString,
                QString, QString, QString,
                QString, QString,
                bool, bool, bool, bool,
                QString, QString,
                QString);
private:
    QString hAnsi;
    QString ascii;
    QString eastAsia;
    QString hint;
    QString hAnsiTheme;
    QString asciiTheme;
    QString eastAsiaTheme;

    QString colorVal;
    QString highlightColorVal;

    bool Bold;
    bool BoldComplexScript;
    bool Italic;
    bool ItalicComplexScript;

    QString sizeVal;
    QString sizeComplexScriptVal;

    QString hyperlinkID;
};


class ParagraphProperty{
    friend class WordParser;

public:
    ParagraphProperty(QString,
                      QString,
                      QString, QString, QString,
                      QString, QString, QString);
private:
    QString styleId;

    QString justification;

    QString spacingBefore;
    QString spacingAfter;
    QString spacingLine;

    QString indentationFirstLine;
    QString indentationLeft;
    QString indentationRight;
};


class SectionProperty{
    friend class WordParser;

public:
    SectionProperty(QString, QString,
                    QString, QString,
                    QString, QString, QString, QString);
private:
    QString width;
    QString height;

    QString header;
    QString footer;

    QString left;
    QString right;
    QString top;
    QString bottom;
};


class Style{
    friend class WordParser;

public:
    Style(QString,
          shared_ptr<ParagraphProperty>,
          shared_ptr<RunProperty>);
private:
    shared_ptr<ParagraphProperty> pPr;
    shared_ptr<RunProperty> rPr;
    QString name;
};

class RunInfo{
    friend class WordParser;

public:
    RunInfo(QString, shared_ptr<RunProperty>);
private:
    QString text;
    shared_ptr<RunProperty> rPr;
};

class ParagraphInfo{
    friend class WordParser;

public:
    ParagraphInfo(vector<shared_ptr<RunInfo>>, shared_ptr<ParagraphProperty>);
private:
    vector<shared_ptr<RunInfo>> runInfos;
    shared_ptr<ParagraphProperty> pPr;
};

class TableCol{
    friend class WordParser;

public:
    TableCol(vector<shared_ptr<ParagraphInfo>>, QString);
private:
    vector<shared_ptr<ParagraphInfo>> paragraphInfos;
    QString width;
};

class TableRow{
    friend class WordParser;

public:
    TableRow(vector<shared_ptr<TableCol>>, QString);
private:
    vector<shared_ptr<TableCol>> tableCols;
    QString height;

};

class Table{
    friend class WordParser;

public:
    Table(vector<shared_ptr<TableRow>>);
private:
    vector<shared_ptr<TableRow>> tableRows;
};

class Body{
    friend class WordParser;

public:
    Body(vector<variant<shared_ptr<ParagraphInfo>, shared_ptr<Table>>>, shared_ptr<SectionProperty>);
private:
    vector<variant<shared_ptr<ParagraphInfo>, shared_ptr<Table>>> paragraphInfosOrTables;
    shared_ptr<SectionProperty> sectPr;
};

class WordDocument
{
public:
    WordDocument();
    shared_ptr<Body> & getBody();
    shared_ptr<Body> getBody() const;

    map<QString, shared_ptr<Style>> & getStyleMap();
    map<QString, shared_ptr<Style>> getStyleMap() const;
    set<QString> getStyleIDSet() const;

    map<QString, QString> & getHyperlinkIDMap();
    map<QString, QString> getHyperlinkIDMap() const;

    set<QString> & getHyperlinkIDSet();
    set<QString> getHyperlinkIDSet() const;
private:
    shared_ptr<Body> body;

    map<QString, shared_ptr<Style>> styleMap;
    set<QString> styleIDSet = {"1", "2", "3", "4", "5", "6",
                               "af7", // title
                               "af5", // subtitle
                              };

    map<QString, QString> hyperlinkIDMap;
    set<QString> hyperlinkIDSet;
};

#endif // WORDDOCUMENT_H
