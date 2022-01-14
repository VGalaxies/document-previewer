#ifndef WORDPARSER_H
#define WORDPARSER_H

#include "worddocument.h"

class WordParser
{
public:
    WordParser(WordDocument);
    void start();
    QString getRes() const;
private:
    void head();
    void meta();
    void style();
    void page();
    void format();
    void tableFormat();

    void body();
    void div();
    void paragraphOrTable();
    void paragraph(const shared_ptr<ParagraphInfo> &);
    void run(const vector<shared_ptr<RunInfo>> &);

    void table(const shared_ptr<Table> &);
    void tableRow(const shared_ptr<TableRow> &);
    void tableCol(const shared_ptr<TableCol> &);

    void paragraphPropertyHierarchy();
    void runPropertyHierarchy();

    WordDocument document;
    QString res;
    QString wordSectionName = "WordSection1";
    QString defaultFontName = "等线";
    QString defaultFontSize = "22";

    // not found, set manually
    QString defaultMissingFormat = "margin: 0cm;\
margin-bottom: .0001pt;\
text-align: justify;\
text-justify: inter-ideograph;";

    vector<shared_ptr<ParagraphProperty>> pPrHierarchy;
    vector<shared_ptr<RunProperty>> rPrHierarchy;
};

#endif // WORDPARSER_H
