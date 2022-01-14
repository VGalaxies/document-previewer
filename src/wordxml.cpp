#include "wordxml.h"

WordXML::WordXML(QString path)
{
    this->path = path;
}

void WordXML::readProcess(QXmlStreamReader * reader)
{
    reader->readNextStartElement();
    assert(reader->name().toString() == "package");

    while(!reader->atEnd() || !reader->hasError())
    {
        reader->readNextStartElement();
        if(reader->name().toString() == "part"){
            QXmlStreamAttributes attribute = reader->attributes();
            if(attribute.hasAttribute("pkg:name")){
                if(attribute.value("pkg:name").toString() == "/word/document.xml"){
                    reader->readNextStartElement();
                    assert(reader->name().toString() == "xmlData");
                    reader->readNextStartElement();
                    assert(reader->name().toString() == "document");
                    reader->readNextStartElement();
                    assert(reader->name().toString() == "body");
                    reader->readNextStartElement();
                    while(reader->name().toString() != "sectPr"){
                        //qDebug() << reader->name().toString();
                        reader->readNextStartElement();
                    }
                }
            }
        }
    }
}

shared_ptr<RunProperty> WordXML::domRunProperty(QDomNodeList * runProperties)
{
    QString hAnsi, ascii, eastAsia, hint, hAnsiTheme, asciiTheme, eastAsiaTheme;
    QString colorVal, highlightColorVal;
    bool Bold = false, BoldComplexScript = false, Italic = false, ItalicComplexScript = false;
    QString sizeVal, sizeComplexScriptVal;

    for(int i = 0; i < runProperties->size(); ++i){
        QDomElement runProperty = runProperties->at(i).toElement();
        if(runProperty.tagName() == "w:rFonts"){
            hAnsi = runProperty.attribute("w:hAnsi", "");
            ascii = runProperty.attribute("w:ascii", "");
            eastAsia = runProperty.attribute("w:eastAsia", "");
            hint = runProperty.attribute("w:hint", "");
            //qDebug() << "hAnsi: " << hAnsi << " ascii: " << ascii << " eastAsia: " << eastAsia << " hint: " << hint;

            hAnsiTheme = runProperty.attribute("w:hAnsiTheme", "");
            asciiTheme = runProperty.attribute("w:asciiTheme", "");
            eastAsiaTheme = runProperty.attribute("w:eastAsiaTheme", "");
            //qDebug() << "hAnsiTheme: " << hAnsiTheme << " asciiTheme: " << asciiTheme << " eastAsiaTheme: " << eastAsiaTheme;
        }else if(runProperty.tagName() == "w:color"){
            colorVal = runProperty.attribute("w:val", "");
            qDebug() << "colorVal: " << colorVal;
        }else if(runProperty.tagName() == "w:highlight"){
            highlightColorVal = runProperty.attribute("w:val", "");
            qDebug() << "highlightColorVal: " << highlightColorVal;
        }else if(runProperty.tagName() == "w:b"){
            Bold = true;
            //qDebug() << "Bold";
        }else if(runProperty.tagName() == "w:bCs"){
            BoldComplexScript = true;
            //qDebug() << "BoldComplexScript";
        }else if(runProperty.tagName() == "w:i"){
            Italic = true;
            //qDebug() << "Italic";
        }else if(runProperty.tagName() == "w:iCs"){
            ItalicComplexScript = true;
            //qDebug() << "ItalicComplexScript";
        }else if(runProperty.tagName() == "w:sz"){
            sizeVal = runProperty.attribute("w:val", "");
            //qDebug() << "sizeVal: " << sizeVal;
        }else if(runProperty.tagName() == "w:szCs"){
            sizeComplexScriptVal = runProperty.attribute("w:val", "");
            //qDebug() << "sizeComplexScriptVal: " << sizeComplexScriptVal;
        }
    }

    return make_shared<RunProperty>(hAnsi, ascii, eastAsia, hint,
                                    hAnsiTheme, asciiTheme, eastAsiaTheme,
                                    colorVal, highlightColorVal,
                                    Bold, BoldComplexScript, Italic, ItalicComplexScript,
                                    sizeVal, sizeComplexScriptVal,
                                    this->hyperlinkID);
}

shared_ptr<RunInfo> WordXML::domRunInfo(QDomNodeList * runInfos) // children of run
{
    QString text = "";
    shared_ptr<RunProperty> rPr;

    //qDebug() << "******** run start ********";
    for (int i = 0; i < runInfos->size(); ++i){
        QDomElement runInfo = runInfos->at(i).toElement();
        if(runInfo.tagName() == "w:rPr"){
            QDomNodeList runProperties = runInfo.childNodes();
            rPr = domRunProperty(&runProperties);
        }else if(runInfo.tagName() == "w:t"){
            text = runInfo.text();
            //qDebug() << text;
        }
    }
    //qDebug() << "******** run end ********";

    return make_shared<RunInfo>(text, rPr);
}

shared_ptr<ParagraphProperty> WordXML::domParagraphProperty(QDomNodeList * paragraphProperties)
{
    QString styleId;
    QString justification;
    QString spacingBefore;
    QString spacingAfter;
    QString spacingLine;
    QString indentationFirstLine;
    QString indentationLeft;
    QString indentationRight;

    for (int i = 0; i < paragraphProperties->size(); ++i){
        QDomElement paragraphProperty = paragraphProperties->at(i).toElement();
        if(paragraphProperty.tagName() == "w:jc"){
            justification = paragraphProperty.attribute("w:val", "");
            //qDebug() << "justification: " << justification;
        }else if(paragraphProperty.tagName() == "w:pStyle"){
            styleId = paragraphProperty.attribute("w:val", "");
            //qDebug() << "styleId: " << styleId;
        }else if(paragraphProperty.tagName() == "w:spacing"){
            spacingBefore = paragraphProperty.attribute("w:before", "");
            spacingAfter = paragraphProperty.attribute("w:after", "");
            spacingLine = paragraphProperty.attribute("w:line", "");
            //qDebug() << "before: " << before << "after: " << after;
        }else if(paragraphProperty.tagName() == "w:ind"){
            indentationFirstLine = paragraphProperty.attribute("w:firstLine", "");
            indentationLeft = paragraphProperty.attribute("w:left", "");
            indentationRight = paragraphProperty.attribute("w:right", "");
        }
    }

    return make_shared<ParagraphProperty>(styleId,
                                          justification,
                                          spacingBefore, spacingAfter, spacingLine,
                                          indentationFirstLine, indentationLeft, indentationRight);
}

shared_ptr<ParagraphInfo> WordXML::domParagraphInfo(QDomNodeList * paragraphInfos) // children of paragraph
{
    shared_ptr<ParagraphProperty> pPr;
    vector<shared_ptr<RunInfo>> runNodes;

    //qDebug() << "******** paragraph start ********";
    for (int i = 0; i < paragraphInfos->size(); ++i){
        QDomElement paragraphInfo = paragraphInfos->at(i).toElement();
        if(paragraphInfo.tagName() == "w:pPr"){
            QDomNodeList paragraphProperties = paragraphInfo.childNodes();
            pPr = domParagraphProperty(&paragraphProperties);
        }else if(paragraphInfo.tagName() == "w:r"){
            QDomNodeList runInfos = paragraphInfo.childNodes();
            runNodes.push_back(domRunInfo(&runInfos));
        }else if(paragraphInfo.tagName() == "w:hyperlink"){
            hyperlinkID = paragraphInfo.attribute("r:id", "");
            assert(hyperlinkID != "");
            document.getHyperlinkIDSet().insert(hyperlinkID);
            QDomNodeList hyperlinkRuns = paragraphInfo.childNodes();
            for (int j = 0; j < hyperlinkRuns.size(); ++j){
                QDomNodeList hyperlinkRunInfos = hyperlinkRuns.at(j).toElement().childNodes();
                runNodes.push_back(domRunInfo(&hyperlinkRunInfos));
            }
            hyperlinkID = "";
        }
    }
    //qDebug() << "******** paragraph end ********";
    //qDebug() << "";

    return make_shared<ParagraphInfo>(runNodes, pPr);
}

shared_ptr<SectionProperty> WordXML::domSectionProperty(QDomNodeList * sectionProperties)
{
    QString width;
    QString height;

    QString header;
    QString footer;

    QString left;
    QString right;
    QString top;
    QString bottom;

    for (int i = 0; i < sectionProperties->size(); ++i){
        QDomElement sectionProperty = sectionProperties->at(i).toElement();
        if(sectionProperty.tagName() == "w:pgSz"){
            width = sectionProperty.attribute("w:w", "");
            height = sectionProperty.attribute("w:h", "");
            //qDebug() << "width: " << width;
            //qDebug() << "height: " << height;
        }else if(sectionProperty.tagName() == "w:pgMar"){
            header = sectionProperty.attribute("w:header", "");
            footer = sectionProperty.attribute("w:footer", "");
            left = sectionProperty.attribute("w:left", "");
            right = sectionProperty.attribute("w:right", "");
            top = sectionProperty.attribute("w:top", "");
            bottom = sectionProperty.attribute("w:bottom", "");
            //qDebug() << "header: " << header;
            //qDebug() << "footer: " << footer;
            //qDebug() << "left: " << left;
            //qDebug() << "right: " << right;
            //qDebug() << "top: " << top;
            //qDebug() << "bottom: " << bottom;
        }
    }

    return make_shared<SectionProperty>(width, height,
                                        header, footer,
                                        left, right, top, bottom);
}

shared_ptr<TableCol> WordXML::domTableColInfo(QDomNodeList * tableColInfos)
{
    vector<shared_ptr<ParagraphInfo>> paragraphNodes;
    QString width = "";

    for (int i = 0; i < tableColInfos->size(); ++i){
        QDomElement tableColInfo = tableColInfos->at(i).toElement();
        if(tableColInfo.tagName() == "w:p"){
            QDomNodeList paragraphInfos = tableColInfo.childNodes();
            paragraphNodes.push_back(domParagraphInfo(&paragraphInfos));
        }else if(tableColInfo.tagName() == "w:tcPr"){
            QDomNodeList tableColProperties = tableColInfo.childNodes();
            for (int j = 0; j < tableColProperties.size(); ++j) {
                QDomElement tableColProperty = tableColProperties.at(j).toElement();
                if(tableColProperty.tagName() == "w:tcW"){
                    width = tableColProperty.attribute("w:w", "");
                }
            }
        }
    }

    return make_shared<TableCol>(paragraphNodes, width);
}

shared_ptr<TableRow> WordXML::domTableRowInfo(QDomNodeList * tableRowInfos)
{
    vector<shared_ptr<TableCol>> tableCols;
    QString height = "";

    for (int i = 0; i < tableRowInfos->size(); ++i){
        QDomElement tableRowInfo = tableRowInfos->at(i).toElement();
        if(tableRowInfo.tagName() == "w:tc"){
            QDomNodeList tableColInfos = tableRowInfo.childNodes();
            tableCols.push_back(domTableColInfo(&tableColInfos));
        }else if(tableRowInfo.tagName() == "w:trPr"){
            QDomNodeList tableRowProperties = tableRowInfo.childNodes();
            for (int j = 0; j < tableRowProperties.size(); ++j) {
                QDomElement tableRowProperty = tableRowProperties.at(j).toElement();
                if(tableRowProperty.tagName() == "w:trHeight"){
                    height = tableRowProperty.attribute("w:val", "");
                }
            }
        }
    }

    return make_shared<TableRow>(tableCols, height);
}

shared_ptr<Table> WordXML::domTableInfo(QDomNodeList * tableInfos)
{
    vector<shared_ptr<TableRow>> tableRows;

    for (int i = 0; i < tableInfos->size(); ++i){
        QDomElement tableInfo = tableInfos->at(i).toElement();
        if(tableInfo.tagName() == "w:tr"){
            QDomNodeList tableRowInfos = tableInfo.childNodes();
            tableRows.push_back(domTableRowInfo(&tableRowInfos));
        }
    }

    return make_shared<Table>(tableRows);
}

void WordXML::domBodyInfo(QDomNodeList * bodyInfos) // children of body
{
    shared_ptr<SectionProperty> sectPr;
    vector<variant<shared_ptr<ParagraphInfo>, shared_ptr<Table>>> paragraphInfosOrTables;

    //qDebug() << "******** body start ********";
    for (int i = 0; i < bodyInfos->size(); ++i){
        QDomElement bodyInfo = bodyInfos->at(i).toElement();
        if(bodyInfo.tagName() == "w:sectPr"){
            QDomNodeList sectionProperties = bodyInfo.childNodes();
            sectPr = domSectionProperty(&sectionProperties);
        }else if(bodyInfo.tagName() == "w:p"){
            QDomNodeList paragraphInfos = bodyInfo.childNodes();
            paragraphInfosOrTables.push_back(domParagraphInfo(&paragraphInfos));
        }else if(bodyInfo.tagName() == "w:tbl"){
            QDomNodeList tableInfos = bodyInfo.childNodes();
            paragraphInfosOrTables.push_back(domTableInfo(&tableInfos));
        }
    }
    //qDebug() << "******** body end ********";

    document.getBody() = make_shared<Body>(paragraphInfosOrTables, sectPr);
}

void WordXML::domStyleDefault(QDomNodeList * docDefaults)
{
    QString styleID = "default";
    QString styleName = "Default";
    shared_ptr<ParagraphProperty> pPr = nullptr;
    shared_ptr<RunProperty> rPr;

    //qDebug() << "******** docDefaults style start ********";
    for (int i = 0; i < docDefaults->size(); ++i){
        QDomElement docDefault = docDefaults->at(i).toElement();
        if(docDefault.tagName() == "w:rPrDefault"){
            assert(docDefault.childNodes().at(0).toElement().tagName() == "w:rPr");
            QDomNodeList rPrDefaults = docDefault.childNodes().at(0).toElement().childNodes();
            rPr = domRunProperty(&rPrDefaults);
        }else if(docDefault.tagName() == "w:pPrDefault"){
            // TODO
//            assert(docDefault.childNodes().at(0).toElement().tagName() == "w:pPr");
//            QDomNodeList pPrDefaults = docDefault.childNodes().at(0).toElement().childNodes();
//            domParagraphProperty(&pPrDefaults);
        }
    }
    //qDebug() << "******** docDefaults style end ********";

    document.getStyleMap().insert(make_pair(styleID,
                                            make_shared<Style>(styleName, pPr, rPr)));
}

void WordXML::domStyleID(QDomElement * style)
{
    QString styleID = style->attribute("w:styleId", "");
    set<QString> styleIDSet = document.getStyleIDSet();
    if(styleIDSet.find(styleID) != styleIDSet.end()){
        QString styleName;
        shared_ptr<ParagraphProperty> pPr;
        shared_ptr<RunProperty> rPr;

        QDomNodeList styleChildren = style->childNodes();
        for (int i = 0; i < styleChildren.size(); ++i){
            QDomElement styleChild = styleChildren.at(i).toElement();
            if(styleChild.tagName() == "w:name"){
                styleName = styleChild.attribute("w:val", "");
            }else if(styleChild.tagName() == "w:pPr"){
                QDomNodeList paragraphProperties = styleChild.childNodes();
                pPr = domParagraphProperty(&paragraphProperties);
            }else if(styleChild.tagName() == "w:rPr"){
                QDomNodeList runProperties = styleChild.childNodes();
                rPr = domRunProperty(&runProperties);
            }
        }

        document.getStyleMap().insert(make_pair(styleID,
                                                make_shared<Style>(styleName, pPr, rPr)));
        //qDebug() << "";
    }
}

void WordXML::domStyleInfo(QDomNodeList * styleInfos)
{
    //qDebug() << "******** style start ********";
    for (int i = 0; i < styleInfos->size(); ++i){
        QDomElement styleInfo = styleInfos->at(i).toElement();
        if(styleInfo.tagName() == "w:docDefaults"){
            QDomNodeList docDefaults = styleInfo.childNodes();
            domStyleDefault(&docDefaults);
        }else if(styleInfo.tagName() == "w:style"){
            domStyleID(&styleInfo);
        }
    }
    //qDebug() << "******** style end ********";
}

void WordXML::domRelationshipInfo(QDomNodeList * relationshipInfos)
{
    for (int i = 0; i < relationshipInfos->size(); ++i){
        QDomElement relationshipInfo = relationshipInfos->at(i).toElement();
        assert(relationshipInfo.tagName() == "Relationship");

        QString id = relationshipInfo.attribute("Id", "");
        assert(id != "");

        auto hyperlinkIDSet = document.getHyperlinkIDSet();
        map<QString, QString> & hyperlinkIDMap = document.getHyperlinkIDMap();
        if (hyperlinkIDSet.find(id) != hyperlinkIDSet.end()) {
            QString target = relationshipInfo.attribute("Target", "");
            assert(target != "");
            hyperlinkIDMap.insert(make_pair(id, target));
        }
    }
}

void WordXML::domPart(QDomDocument * doc)
{
    QDomElement root = doc->documentElement();
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QString name = node.toElement().tagName(); // pkg:part
        QString attr = node.toElement().attribute("pkg:name", "");
        if(name == "pkg:part" && attr == "/word/document.xml"){
            QDomElement firstElement = node.childNodes().at(0).toElement();
            assert(firstElement.tagName() == "pkg:xmlData");
            firstElement = firstElement.childNodes().at(0).toElement();
            assert(firstElement.tagName() == "w:document");
            firstElement = firstElement.childNodes().at(0).toElement();
            assert(firstElement.tagName() == "w:body");
            QDomNodeList bodyInfos = firstElement.childNodes();
            domBodyInfo(&bodyInfos);
        }else if(name == "pkg:part" && attr == "/word/styles.xml"){
            QDomElement firstElement = node.childNodes().at(0).toElement();
            assert(firstElement.tagName() == "pkg:xmlData");
            firstElement = firstElement.childNodes().at(0).toElement();
            assert(firstElement.tagName() == "w:styles");
            QDomNodeList styleInfos = firstElement.childNodes();
            domStyleInfo(&styleInfos);
        }else if(name == "pkg:part" && attr == "/word/_rels/document.xml.rels"){
            QDomElement firstElement = node.childNodes().at(0).toElement();
            assert(firstElement.tagName() == "pkg:xmlData");
            firstElement = firstElement.childNodes().at(0).toElement();
            assert(firstElement.tagName() == "Relationships");
            QDomNodeList relationshipInfos = firstElement.childNodes();
            domRelationshipInfo(&relationshipInfos);
        }
        node = node.nextSibling();
    }
}

void WordXML::start()
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {  // 以只读模式打开文件
//        QXmlStreamReader reader(&file);
//        readProcess(&reader);

        QDomDocument doc;
        doc.setContent(&file);
        domPart(&doc);

        file.close();  // 关闭文件
    }
}

WordDocument WordXML::getDocument() const
{
    return document;
}
