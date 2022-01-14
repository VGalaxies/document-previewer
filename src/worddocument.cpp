#include "worddocument.h"

WordDocument::WordDocument()
{

}

shared_ptr<Body> & WordDocument::getBody()
{
    return body;
}

map<QString, shared_ptr<Style>> & WordDocument::getStyleMap()
{
    return styleMap;
}

shared_ptr<Body> WordDocument::getBody() const
{
    return body;
}

map<QString, shared_ptr<Style>> WordDocument::getStyleMap() const
{
    return styleMap;
}

set<QString> WordDocument::getStyleIDSet() const
{
    return styleIDSet;
}

set<QString> & WordDocument::getHyperlinkIDSet()
{
    return hyperlinkIDSet;
}

set<QString> WordDocument::getHyperlinkIDSet() const
{
    return hyperlinkIDSet;
}

map<QString, QString> WordDocument::getHyperlinkIDMap() const
{
    return hyperlinkIDMap;
}

map<QString, QString> & WordDocument::getHyperlinkIDMap()
{
    return hyperlinkIDMap;
}

Style::Style(QString name, shared_ptr<ParagraphProperty> pPr, shared_ptr<RunProperty> rPr)
{
    this->name = name;
    this->pPr = pPr;
    this->rPr = rPr;
}

RunProperty::RunProperty(QString hAnsi, QString ascii, QString eastAsia, QString hint,
                         QString hAnsiTheme, QString asciiTheme, QString eastAsiaTheme,
                         QString colorVal, QString highlightColorVal,
                         bool Bold, bool BoldComplexScript, bool Italic, bool ItalicComplexScript,
                         QString sizeVal, QString sizeComplexScriptVal,
                         QString hyperlinkID)
{
    this->hAnsi = hAnsi;
    this->ascii = ascii;
    this->eastAsia = eastAsia;
    this->hint = hint;
    this->hAnsiTheme = hAnsiTheme;
    this->asciiTheme = asciiTheme;
    this->eastAsiaTheme = eastAsiaTheme;

    this->colorVal = colorVal;
    this->highlightColorVal = highlightColorVal;

    this->Bold = Bold;
    this->BoldComplexScript = BoldComplexScript;
    this->Italic = Italic;
    this->ItalicComplexScript = ItalicComplexScript;

    this->sizeVal = sizeVal;
    this->sizeComplexScriptVal = sizeComplexScriptVal;

    this->hyperlinkID = hyperlinkID;
}

ParagraphProperty::ParagraphProperty(QString styleId,
                                     QString justification,
                                     QString spacingBefore, QString spacingAfter, QString spacingLine,
                                     QString indentationFirstLine,  QString indentationLeft, QString indentationRight)
{
    this->styleId = styleId;

    this->justification = justification;

    this->spacingBefore = spacingBefore;
    this->spacingAfter = spacingAfter;
    this->spacingLine = spacingLine;

    this->indentationFirstLine = indentationFirstLine;
    this->indentationLeft = indentationLeft;
    this->indentationRight = indentationRight;
}

SectionProperty::SectionProperty(QString width, QString height,
                                 QString header, QString footer,
                                 QString left, QString right, QString top, QString bottom)
{
    this->width = width;
    this->height = height;

    this->header = header;
    this->footer = footer;

    this->left = left;
    this->right = right;
    this->top = top;
    this->bottom = bottom;
}

RunInfo::RunInfo(QString text, shared_ptr<RunProperty> rPr)
{
    this->text = text;
    this->rPr = rPr;
}

ParagraphInfo::ParagraphInfo(vector<shared_ptr<RunInfo>> runInfos, shared_ptr<ParagraphProperty> pPr)
{
    this->runInfos = runInfos;
    this->pPr = pPr;
}

TableCol::TableCol(vector<shared_ptr<ParagraphInfo>> paragraphInfos, QString width)
{
    this->paragraphInfos = paragraphInfos;
    this->width = width;
}

TableRow::TableRow(vector<shared_ptr<TableCol>> tableCols, QString height)
{
    this->tableCols = tableCols;
    this->height = height;
}

Table::Table(vector<shared_ptr<TableRow>> tableRows)
{
    this->tableRows = tableRows;
}

Body::Body(vector<variant<shared_ptr<ParagraphInfo>, shared_ptr<Table>>> paragraphInfosOrTables, shared_ptr<SectionProperty> sectPr)
{
    this->paragraphInfosOrTables = paragraphInfosOrTables;
    this->sectPr = sectPr;
}
