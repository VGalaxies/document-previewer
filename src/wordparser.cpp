#include "wordparser.h"

WordParser::WordParser(WordDocument document)
{
    this->document = document;
}

void WordParser::format()
{
    auto styleMap = document.getStyleMap();
    for(auto styleMapItem = styleMap.cbegin();
        styleMapItem != styleMap.cend(); ++styleMapItem){
        auto styleID = styleMapItem->first;
        auto style = styleMapItem->second;

        QString spacingBefore = "";
        QString spacingAfter = "";
        QString spacingLine = "";

        QString justification = "";
        QString indentationFirstLine = "";
        QString indentationLeft = "";
        QString indentationRight = "";

        if(style->pPr != nullptr){ // !!!
            spacingBefore = style->pPr->spacingBefore;
            spacingAfter = style->pPr->spacingAfter;
            spacingLine = style->pPr->spacingLine;

            justification = style->pPr->justification;
            indentationFirstLine = style->pPr->indentationFirstLine;
            indentationLeft = style->pPr->indentationLeft;
            indentationRight = style->pPr->indentationRight;
        }

        QString size = QString("%1").arg(style->rPr->sizeVal.toDouble() / 2);
        size = (size == "0") ? defaultFontSize : size;

        auto eastAsiaTheme = style->rPr->eastAsiaTheme;
        auto eastAsia = style->rPr->eastAsia;
        QString font;
        if(eastAsiaTheme == "minorEastAsia" || eastAsiaTheme == "majorEastAsia" || eastAsiaTheme == ""){
            if(eastAsia == ""){
                font = defaultFontName;
            }else{
                font = eastAsia;
            }
        }else{
            font = eastAsiaTheme;
        }

        bool isBold = style->rPr->Bold;
        bool isItalic = style->rPr->Italic;

        if(styleID == "default"){
            res.append("p.Default,li.Default,div.Default {");
            res.append(defaultMissingFormat);
        }else if(styleID == "af7"){
            res.append("p.Title,li.Title,div.Title {");
        }else if(styleID == "af5"){
            res.append("p.Subtitle,li.Subtitle,div.Subtitle {");
        }else{ // assume h1 ~ h6
            res.append("h").append(styleID).append(" {");
        }


        // rPr
        res.append("font-size: ").append(size).append("pt;");
        res.append("font-family: ").append(font).append(";");

        if(isBold) res.append("font-weight: bold;");
        else res.append("font-weight: normal;");

        if(isItalic) res.append("font-style: italic;");

        // pPr
        if(spacingBefore != ""){
            spacingBefore = QString("%1").arg(spacingBefore.toDouble() / 20);
            res.append("margin-top: ").append(spacingBefore).append("pt;");
        }
        if(spacingAfter != ""){
            spacingAfter = QString("%1").arg(spacingAfter.toDouble() / 20);
            res.append("margin-bottom: ").append(spacingAfter).append("pt;");
        }
        if(spacingLine != ""){
            spacingLine = QString("%1").arg(spacingLine.toDouble() / 20 / 12 * 100);
            res.append("line-height: ").append(spacingLine).append("%;");
        }
        if(indentationFirstLine != ""){
            indentationFirstLine = QString("%1").arg(indentationFirstLine.toDouble() / 20);
            res.append("text-indent: ").append(indentationFirstLine).append("pt;");
        }
        if(indentationLeft != ""){
            indentationRight = QString("%1").arg(indentationLeft.toDouble() / 20);
            res.append("text-indent: ").append(indentationLeft).append("pt;");
        }
        if(indentationRight != ""){
            indentationRight = QString("%1").arg(indentationRight.toDouble() / 20);
            res.append("text-indent: ").append(indentationRight).append("pt;");
        }
        if(justification != ""){
            res.append("text-align: ").append(justification).append(";");
        }

        res.append("}");
    }
}

void WordParser::page()
{
    auto sectPr = document.getBody()->sectPr;

    QString width = QString("%1").arg(sectPr->width.toDouble() / 20);
    QString height = QString("%1").arg(sectPr->height.toDouble() / 20);

//    QString header = sectPr->header;
//    QString footer = sectPr->footer;

    QString left = QString("%1").arg(sectPr->left.toDouble() / 20);
    QString right = QString("%1").arg(sectPr->right.toDouble() / 20);
    QString top = QString("%1").arg(sectPr->top.toDouble() / 20);
    QString bottom = QString("%1").arg(sectPr->bottom.toDouble() / 20);

    res.append("@page ").append(wordSectionName).append(" {");
    res.append("size: ").append(width).append("pt ").
            append(height).append("pt;");
    res.append("margin: ").append(top).append("pt ").
            append(right).append("pt ").
            append(bottom).append("pt ").
            append(left).append("pt;}");

    res.append("div.").append(wordSectionName).append(" {page: ").
            append(wordSectionName).append(";}");
}

void WordParser::tableFormat()
{
    // set manually
    res.append("table, table tr th, table tr td { border: 1px solid #000000; }");
    res.append("table { border-collapse: collapse; }");
}

void WordParser::style()
{
    res.append("<style>");
    tableFormat();
    format();
    page();
    res.append("</style>");
}

void WordParser::meta()
{
    res.append("<meta charset=\"UTF-8\">");
}

void WordParser::head()
{
    res.append("<head>");
    meta();
    style();
    res.append("</head>");
}

void WordParser::paragraphPropertyHierarchy()
{
    bool styleSt = false;

    for (auto pPr = pPrHierarchy.crbegin(); pPr != pPrHierarchy.crend(); ++pPr){
        // TODO hierarchy
        if(pPr->get()->justification != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            res.append("text-align:").append(pPr->get()->justification).append(";");
        }
        if(pPr->get()->spacingBefore != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString spacingBefore = QString("%1").arg(pPr->get()->spacingBefore.toDouble() / 20);
            res.append("margin-top:").append(spacingBefore).append("px;");
        }
        if(pPr->get()->spacingAfter != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString spacingAfter = QString("%1").arg(pPr->get()->spacingAfter.toDouble() / 20);
            res.append("margin-bottom:").append(spacingAfter).append("px;");
        }
        if(pPr->get()->spacingLine != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString spacingLine = QString("%1").arg(pPr->get()->spacingLine.toDouble() / 20 / 12 * 100);
            res.append("line-height:").append(spacingLine).append("%;");
        }
        if(pPr->get()->indentationFirstLine != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString indentationFirstLine = QString("%1").arg(pPr->get()->indentationFirstLine.toDouble() / 20);
            res.append("text-indent:").append(indentationFirstLine).append("px;");
        }
        if(pPr->get()->indentationLeft != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString indentationLeft = QString("%1").arg(pPr->get()->indentationLeft.toDouble() / 20);
            res.append("margin-left:").append(indentationLeft).append("px;");
        }
        if(pPr->get()->indentationRight != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString indentationRight = QString("%1").arg(pPr->get()->indentationRight.toDouble() / 20);
            res.append("margin-right:").append(indentationRight).append("px;");
        }
    }

    if(styleSt) res.append("'");
}

void WordParser::runPropertyHierarchy()
{
    bool styleSt = false;

    for (auto rPr = rPrHierarchy.crbegin(); rPr != rPrHierarchy.crend(); ++rPr){
        // TODO hierarchy
        if(rPr->get()->ascii != ""){ // assume
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            res.append("font-family:").append("\"").append(rPr->get()->ascii).append("\";");
        }
        if(rPr->get()->Bold){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            res.append("font-weight:bold;");
        }
        if(rPr->get()->Italic){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            res.append("font-style:italic;");
        }
        if(rPr->get()->colorVal != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString attr = rPr->get()->colorVal;
            QChar first = attr.at(0);
            if(first.isDigit() || first.isUpper()) // 0 ~ 9, A ~ F
                res.append("color:#");
            else
                res.append("color:");
            res.append(rPr->get()->colorVal).append(";");
        }
        if(rPr->get()->highlightColorVal != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString attr = rPr->get()->highlightColorVal;
            QChar first = attr.at(0);
            if(first.isDigit() || first.isUpper()) // 0 ~ 9, A ~ F
                res.append("background-color:#");
            else
                res.append("background-color:");
            res.append(rPr->get()->highlightColorVal).append(";");
        }
        if(rPr->get()->sizeVal != ""){
            if(!styleSt){
                res.append(" style='");
                styleSt = true;
            }
            QString size = QString("%1").arg(rPr->get()->sizeVal.toDouble() / 2);
            res.append("font-size:").append(size).append("pt;");
        }
    }

    if(styleSt) res.append("'");
}

void WordParser::run(const vector<shared_ptr<RunInfo>> & runInfos)
{
    if (runInfos.size() == 0){ // 空行
        res.append("<br>");
        return;
    }

    for (auto runInfo = runInfos.cbegin(); runInfo != runInfos.cend(); ++runInfo){
        auto text = runInfo->get()->text;
        auto rPr = runInfo->get()->rPr;

        bool isHyperlink = false;

        res.append("<span");
        if(rPr) {
            rPrHierarchy.push_back(rPr);
            runPropertyHierarchy();
        }
        res.append(">");

        if (rPr) { // not consider hierarchy
            QString hyperlinkID = rPr->hyperlinkID;
            if (hyperlinkID != "") {
                QString hyperlink = document.getHyperlinkIDMap()[hyperlinkID];
                res.append("<a href=").append("\"").append(hyperlink).append("\">");
                isHyperlink = true;
            }
        }

        res.append(text);

        if(isHyperlink) res.append("</a>");
        res.append("</span>");

        if(rPr) rPrHierarchy.pop_back();
    }
}

void WordParser::paragraph(const shared_ptr<ParagraphInfo> & paragraphInfo)
{
    auto pPr = paragraphInfo.get()->pPr;
    QString styleId = "";
    if(pPr) {
        styleId = pPr.get()->styleId;
        pPrHierarchy.push_back(pPr);
    }

    auto styleIDSet = document.getStyleIDSet();

    if(styleId == "" || styleIDSet.find(styleId) == styleIDSet.end()){ // or assert(0)
        res.append("<p class=Default");
        paragraphPropertyHierarchy();
        res.append(">");

        run(paragraphInfo.get()->runInfos);

        res.append("</p>");
    }else{
        auto style = document.getStyleMap()[styleId];
        QString styleName = style->name;
        if(styleName.contains("heading ")){
            styleName.replace("heading ", "h");
            res.append("<").append(styleName).append(">");

            run(paragraphInfo.get()->runInfos);

            res.append("</").append(styleName).append(">");
        }else{
            res.append("<p class=").append(style->name).append(">");

            run(paragraphInfo.get()->runInfos);

            res.append("</p>");
        }
    }

    if(pPr) pPrHierarchy.pop_back();
}

void WordParser::tableCol(const shared_ptr<TableCol> & tableColInfo)
{
    res.append("<td");

    QString width = tableColInfo->width;
    if(width != ""){
        width = QString("%1").arg(width.toDouble() / 20);
        res.append(" style='width:").append(width).append("pt'>");
    }else{
        res.append(">");
    }

    auto paragraphInfos = tableColInfo->paragraphInfos;

    for (unsigned int i = 0; i < paragraphInfos.size(); ++i){
        const shared_ptr<ParagraphInfo> & paragraphInfo = paragraphInfos[i];
        paragraph(paragraphInfo);
    }
    res.append("</td>");
}

void WordParser::tableRow(const shared_ptr<TableRow> & tableRowInfo)
{
    res.append("<tr");

    QString height = tableRowInfo->height;
    if(height != ""){
        height = QString("%1").arg(height.toDouble() / 20);
        res.append(" style='height:").append(height).append("pt'>");
    }else{
        res.append(">");
    }

    auto tableCols = tableRowInfo->tableCols;

    for (unsigned int i = 0; i < tableCols.size(); ++i){
        const shared_ptr<TableCol> & tableColInfo = tableCols[i];
        tableCol(tableColInfo);
    }
    res.append("</tr>");
}

void WordParser::table(const shared_ptr<Table> & tableInfo)
{
    res.append("<table>");
    auto tableRows = tableInfo->tableRows;
    for (unsigned int i = 0; i < tableRows.size(); ++i){
        const shared_ptr<TableRow> & tableRowInfo = tableRows[i];
        tableRow(tableRowInfo);
    }
    res.append("</table>");
}

void WordParser::paragraphOrTable()
{
    auto paragraphInfosOrTables = document.getBody()->paragraphInfosOrTables;
    for (unsigned int i = 0; i < paragraphInfosOrTables.size(); ++i){
        auto paragraphInfosOrTable = paragraphInfosOrTables[i];
        if(holds_alternative<shared_ptr<ParagraphInfo>>(paragraphInfosOrTable)){
            const shared_ptr<ParagraphInfo> & paragraphInfo = get<shared_ptr<ParagraphInfo>>(paragraphInfosOrTable);
            paragraph(paragraphInfo);
        }else if(holds_alternative<shared_ptr<Table>>(paragraphInfosOrTable)){
            const shared_ptr<Table> & tableInfo = get<shared_ptr<Table>>(paragraphInfosOrTable);
            table(tableInfo);
        }else{
            assert(0);
        }
    }
}

void WordParser::div()
{
    res.append("<div class=").append(wordSectionName).append(">");
    paragraphOrTable();
    res.append("</div>");
}

void WordParser::body()
{
    res.append("<body>");
    div();
    res.append("</body>");
}

void WordParser::start()
{
    res.append("<html>");
    head();
    body();
    res.append("</html>");
}

QString WordParser::getRes() const
{
    return res;
}
