#include "mdparser.h"

#include <cassert>
#include <regex>

using std::regex;
using std::smatch;
using std::regex_search;

MarkdownParser::MarkdownParser(vector<shared_ptr<MarkdownToken>> tokens)
{
    this->tokens = tokens;
}

std::string MarkdownParser::getRes() const
{
    return res;
}

void MarkdownParser::pre()
{
    // TODO title
    res.append("<!DOCTYPE html><head><meta charset=\"UTF-8\"></head><body>"); // utf-8编码
}

void MarkdownParser::post()
{
    res.append("</body></html>");
}

string MarkdownParser::bold(const string & str)
{
    string trans;
    trans.append("<strong>");
    trans.append(str);
    trans.append("</strong>");
    return trans;
}

string MarkdownParser::italic(const string & str)
{
    string trans;
    trans.append("<em>");
    trans.append(str);
    trans.append("</em>");
    return trans;
}

string MarkdownParser::boldAndItalic(const string & str)
{
    string trans;
    trans.append("<em><strong>");
    trans.append(str);
    trans.append("</em></strong>");
    return trans;
}

string MarkdownParser::handleTextLine(const string & str)
{
    string trans;
    unsigned long long current = 0;

    string imagePattern = "!\\[(.*?)\\]\\((.*?)\\)";
    string linkPattern = "\\[(.*?)\\]\\((.*?)\\)";
    string codePattern = "`(.*?)`";

    regex image(imagePattern);
    regex link(linkPattern);
    regex code(codePattern);

    while(current < str.length()){
        switch (str.at(current)) {
        case '*': {
            if(current + 1 < str.length() && str.at(current + 1) == '*'){
                if(current + 2 < str.length() && str.at(current + 2) == '*'){
                    if(current + 3 < str.length() &&
                            str.substr(current + 3).find("***") != string::npos){
                        // ***...***
                        unsigned long long index = str.substr(current + 3).find("***");
                        trans.append(boldAndItalic(str.substr(current + 3, index)));
                        current += index + 6;
                        break;
                    }else if(current + 3 < str.length()){ // ***...
                        trans.append("*");
                        current++;
                        break;
                    }else{ // ***
                        trans.append("*");
                        current++;
                        break;
                    }
                }else if(current + 2 < str.length() &&
                         str.substr(current + 2).find("**") != string::npos){
                    // **...**
                    unsigned long long index = str.substr(current + 2).find("**");
                    trans.append(bold(str.substr(current + 2, index)));
                    current += index + 4; //
                    break;
                }else if(current + 2 < str.length()){ // **...
                    trans.append("*");
                    current++;
                    break;
                }else{ // **
                    trans.append("*");
                    current++;
                    break;
                }
            }else if(current + 1 < str.length() &&
                     str.substr(current + 1).find("*") != string::npos){
                // *...*
                unsigned long long index = str.substr(current + 1).find("*");
                trans.append(italic(str.substr(current + 1, index)));
                current += index + 2; //
                break;
            }else if(current + 1 < str.length()){ // *...
                trans.append("*");
                current++;
                break;
            }else{ // *
                trans.append("*");
                current++;
                break;
            }
        }

        case '!': {
            smatch res;
            string sub = str.substr(current);
            if(regex_search(sub, res, image)){
                string src = res[2];
                string alt = res[1];
                trans.append("<img src=\"" + src + "\" alt=\"" + alt + "\">");
                current += res[0].length();
            }else{
                trans.append("!");
                current++;
            }
            break;
        }

        case '[': {
            smatch res;
            string sub = str.substr(current);
            if(regex_search(sub, res, link)){
                string src = res[2];
                string text = handleTextLine(res[1]);
                trans.append("<a href=\"" + src + "\">" + text + "</a>");
                current += res[0].length();
            }else{
                trans.append("[");
                current++;
            }
            break;
        }

        case '`': {
            smatch res;
            string sub = str.substr(current);
            if(regex_search(sub, res, code)){
                string text = res[1];
                trans.append("<code>" + text + "</code>");
                current += res[0].length();
            }else{
                trans.append("`");
                current++;
            }
            break;
        }

        default: {
            trans.append(1, str.at(current++)); // one time
            break;
        }

        }
    }

    return trans;
}

string MarkdownParser::handleText(const string & str)
{
    string trans;
    unsigned long long current = 0;
    while(str.substr(current).find('\n') != string::npos){
        unsigned long long index = str.substr(current).find('\n');
        trans.append(handleTextLine(str.substr(current, index)));
        trans.append("\n");
        current += index + 1;
    }
    trans.append(handleTextLine(str.substr(current)));
    return trans;
}

void MarkdownParser::handleTokenPre(const shared_ptr<MarkdownToken> & token)
{
    if(listDepth && !inQuote)
        res.append("<li>");

    switch (token->getTokenType()) {
    case MarkdownTokenType::H1: {
        res.append("<h1>");
        break;
    }
    case MarkdownTokenType::H2: {
        res.append("<h2>");
        break;
    }
    case MarkdownTokenType::H3: {
        res.append("<h3>");
        break;
    }
    case MarkdownTokenType::H4: {
        res.append("<h4>");
        break;
    }
    case MarkdownTokenType::H5: {
        res.append("<h5>");
        break;
    }
    case MarkdownTokenType::H6: {
        res.append("<h6>");
        break;
    }
    case MarkdownTokenType::CODE: {
        res.append("<pre><code>");
        break;
    }
    case MarkdownTokenType::QUOTE: {
        res.append("<blockquote>");
        inQuote = true;
        break;
    }
    case MarkdownTokenType::PARA: {
        res.append("<p>");
        break;
    }
    case MarkdownTokenType::UL: {
        listDepth++; // increase depth
        res.append("<ul>");
        break;
    }
    case MarkdownTokenType::OL: {
        listDepth++; // increase depth
        res.append("<ol>");
        break;
    }
    default: {
        assert(0);
    }

    }
}

void MarkdownParser::handleTokenIn(const shared_ptr<MarkdownToken> & token)
{
    switch (token->getTokenType()) {
    case MarkdownTokenType::H1:
    case MarkdownTokenType::H2:
    case MarkdownTokenType::H3:
    case MarkdownTokenType::H4:
    case MarkdownTokenType::H5:
    case MarkdownTokenType::H6:
    case MarkdownTokenType::PARA: {
        res.append(handleText(token->getContent())); // rich text
        break;
    }
    case MarkdownTokenType::CODE: {
        res.append(token->getContent()); // plain text
        break;
    }
    case MarkdownTokenType::QUOTE:
    case MarkdownTokenType::UL:
    case MarkdownTokenType::OL: {
        break; // do nothing
    }
    default: {
        assert(0);
    }

    }
}

void MarkdownParser::handleTokenPost(const shared_ptr<MarkdownToken> & token)
{
    switch (token->getTokenType()) {
    case MarkdownTokenType::H1: {
        res.append("</h1>");
        break;
    }
    case MarkdownTokenType::H2: {
        res.append("</h2>");
        break;
    }
    case MarkdownTokenType::H3: {
        res.append("</h3>");
        break;
    }
    case MarkdownTokenType::H4: {
        res.append("</h4>");
        break;
    }
    case MarkdownTokenType::H5: {
        res.append("</h5>");
        break;
    }
    case MarkdownTokenType::H6: {
        res.append("</h6>");
        break;
    }
    case MarkdownTokenType::CODE: {
        res.append("</pre></code>");
        break;
    }
    case MarkdownTokenType::QUOTE: {
        res.append("</blockquote>");
        inQuote = false;
        break;
    }
    case MarkdownTokenType::PARA: {
        res.append("</p>");
        break;
    }
    case MarkdownTokenType::UL: {
        listDepth--; // decrease depth
        res.append("</ul>");
        break;
    }
    case MarkdownTokenType::OL: {
        listDepth--; // decrease depth
        res.append("</ol>");
        break;
    }
    default: {
        assert(0);
    }

    }

    if(listDepth && !inQuote)
        res.append("</li>");
}

void MarkdownParser::iterate(const shared_ptr<MarkdownToken> & token)
{
    handleTokenPre(token);
    handleTokenIn(token);
    if(token->getChildren().size()){
        for(const shared_ptr<MarkdownToken> & token : token->getChildren()){
            iterate(token);
        }
    }
    handleTokenPost(token);

}

void MarkdownParser::start()
{
    pre();
    for(const shared_ptr<MarkdownToken> & token : tokens){
        iterate(token);
    }
    post();
}

