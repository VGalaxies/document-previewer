#include "mdscanner.h"

#include <cassert>

MarkdownScanner::MarkdownScanner(string ori)
{
    this->ori = ori;
}

vector<shared_ptr<MarkdownToken>> MarkdownScanner::getTokens() const
{
    return this->tokens;
}

bool MarkdownScanner::match(char sub)
{
    if(current == ori.length())
        return false;

    if(ori.at(current) == sub){
        current++;
        return true;
    }else{
        return false;
    }
}

char MarkdownScanner::advance()
{
    assert(current < ori.length());

    return ori.at(current++);
}


bool MarkdownScanner::startwith(unsigned long long index, string sub)
{
    return ori.substr(index, sub.length()) == sub;
}

MarkdownTokenType MarkdownScanner::advanceType()
{
    if(startwith(current, "# ")){
        return MarkdownTokenType::H1;
    }else if(startwith(current, "## ")){
        return MarkdownTokenType::H2;
    }else if(startwith(current, "### ")){
        return MarkdownTokenType::H3;
    }else if(startwith(current, "#### ")){
        return MarkdownTokenType::H4;
    }else if(startwith(current, "##### ")){
        return MarkdownTokenType::H5;
    }else if(startwith(current, "###### ")){
        return MarkdownTokenType::H6;
    }else if(startwith(current, "- ")){
        return MarkdownTokenType::UL;
    }else if(startwith(current, "> ")){
        return MarkdownTokenType::QUOTE;
    }else if(startwith(current, "    ")){
        return MarkdownTokenType::CODE;
    }else if(startwith(current, "```\n")){
        return MarkdownTokenType::CODE;
    }else{
        return MarkdownTokenType::PARA;
    }
}

shared_ptr<MarkdownToken> MarkdownScanner::lastDeepestOpen()
{
    if(this->tokens.size()){
        shared_ptr<MarkdownToken> token = this->tokens.back();
        while(token->getChildren().size()){
            token = token->getChildren().back();
        }
        while(token->getState()){
            token = token->getParent();
            if(!token)
                break;
        }
        return token;
    }
    return nullptr;

}

shared_ptr<MarkdownToken> MarkdownScanner::heading(bool inner, bool list)
{
    int level = 1;
    while(match('#'))
        level++;

    assert(level<=6); // prevent too many
    assert(match(' ')); // 必须有一个空格

    unsigned long long index = ori.substr(current).find('\n') + current;

    shared_ptr<MarkdownToken> ptr(
                new MarkdownToken(
                    static_cast<MarkdownTokenType>(level), // get corresponding tag
                    ori.substr(current, index - current),
                    true));

    if(!inner)
        this->tokens.push_back(ptr);

    this->current = index + 1;

    return ptr;
}

shared_ptr<MarkdownToken> MarkdownScanner::paragraph(bool inner, bool list)
{
    shared_ptr<MarkdownToken> token = lastDeepestOpen();

    unsigned long long length = ori.substr(current).find('\n');
    if(length == 0){ // 只有一个换行符
        while(token){
            // 使用一个空行来开启新的段落
            // nested tokens open -> close
            token->changeState();
            token = token->getParent();
        }
        current += 1;
        return nullptr;
    }

    unsigned long long line_index = length + current, space_index = -1;
    bool linebreak = false;
    if(ori.substr(current, length).find("  \n") != string::npos){
        space_index = ori.substr(current, length).find("  \n") + current;
        linebreak = true;
    }

    while(token){
        if(token->getTokenType() == MarkdownTokenType::PARA)
            break;
        token = token->getParent();
    }

    if(!token || list){ // nullptr -> new token
        shared_ptr<MarkdownToken> ptr(
                    new MarkdownToken(
                        MarkdownTokenType::PARA,
                        ori.substr(current, linebreak ? (space_index - current)
                                                      : (line_index - current))));

        if(linebreak)
            ptr->appendContent("\n"); // 在一行的末尾使用恰好两个空格来换行

        this->current = line_index + 1;

        if(!inner)
            this->tokens.push_back(ptr);

        return ptr;

    }else{ // open -> append token
        token->appendContent("\n"); // 接上
        token->appendContent(ori.substr(current, linebreak ? (space_index - current)
                                                           : (line_index - current)));

        if(linebreak)
            token->appendContent("\n"); // 在一行的末尾使用多于两个空格来换行

        this->current = line_index + 1;

        return nullptr;
    }
}

shared_ptr<MarkdownToken> MarkdownScanner::code_space(bool inner, bool list)
{
    shared_ptr<MarkdownToken> token = lastDeepestOpen();

    while(token){
        if(token->getTokenType() == MarkdownTokenType::CODE)
            break;
        token = token->getParent();
    }

    if(!token){ // nullptr -> new token
        unsigned long long index = ori.substr(current).find('\n') + current;

        shared_ptr<MarkdownToken> ptr(
                    new MarkdownToken(
                        MarkdownTokenType::CODE,
                        ori.substr(current, index - current)));

        this->current = index + 1;

        if(!startwith(current, "    "))
            ptr->changeState();

        if(!inner)
            this->tokens.push_back(ptr);

        return ptr;
    }else{ // open -> append token
        unsigned long long index = ori.substr(current).find('\n') + current;

        token->appendContent("\n");
        token->appendContent(ori.substr(current, index - current));
        this->current = index + 1;

        if(!startwith(current, "    "))
            token->changeState();

        return nullptr;
    }
}

shared_ptr<MarkdownToken> MarkdownScanner::code_fence(bool inner, bool list)
{
    if(ori.substr(current).find("```\n") == string::npos)
        assert(0); // 必须是一对
    unsigned long long index = ori.substr(current).find("```\n") + current;

    shared_ptr<MarkdownToken> ptr(
                new MarkdownToken(
                    MarkdownTokenType::CODE,
                    ori.substr(current, index - current),
                    true));

    this->current = index + 4;

    this->tokens.push_back(ptr); // can not be inner

    return ptr;
}

shared_ptr<MarkdownToken> MarkdownScanner::quote(bool inner, bool list)
{
    assert(match(' ')); // 必须有一个空格

    shared_ptr<MarkdownToken> token = lastDeepestOpen();

    while(token){
        if(token->getTokenType() == MarkdownTokenType::QUOTE)
            break;
        token = token->getParent();
    }

    if(!token){ // new token
        shared_ptr<MarkdownToken> ptr(
                    new MarkdownToken(
                        MarkdownTokenType::QUOTE));

        shared_ptr<MarkdownToken> next = scanToken(true, list);
        next->setParent(ptr);
        ptr->getChildren().push_back(next);

        if(!inner)
            this->tokens.push_back(ptr);

        return ptr;
    }else{
        MarkdownTokenType nextType = advanceType();
        if(nextType == MarkdownTokenType::QUOTE){ // 嵌套引用
            shared_ptr<MarkdownToken> ptr(
                        new MarkdownToken(
                            MarkdownTokenType::QUOTE));
            ptr->setParent(token); // do not forget!
            token->getChildren().push_back(ptr); // modify last deepest open token
        }

        if(nextType == MarkdownTokenType::CODE) assert(0); // 暂不允许内嵌代码

        shared_ptr<MarkdownToken> next = scanToken(true, list); // 嵌套

        if(next){
            // if new token then push back
            // rely on different return statements:
            // - return ptr
            // - return nullptr
            next->setParent(token); // do not forget!
            token->getChildren().push_back(next);
        }

        return nullptr;
    }
}

shared_ptr<MarkdownToken> MarkdownScanner::unorderedList(bool inner, bool list)
{
    assert(match(' ')); // 必须有一个空格

    shared_ptr<MarkdownToken> token = lastDeepestOpen();

    while(token){
        if(token->getTokenType() == MarkdownTokenType::UL)
            break;
        token = token->getParent();
    }

    if(!token){ // new token
        shared_ptr<MarkdownToken> ptr(
                    new MarkdownToken(
                        MarkdownTokenType::UL));

        shared_ptr<MarkdownToken> next = scanToken(true, true);
        next->setParent(ptr);
        ptr->getChildren().push_back(next);

        if(!inner)
            this->tokens.push_back(ptr);

        return ptr;
    }else{
        MarkdownTokenType nextType = advanceType();
        if(nextType == MarkdownTokenType::UL){ // 嵌套无序列表
            shared_ptr<MarkdownToken> ptr(
                        new MarkdownToken(
                            MarkdownTokenType::UL));
            ptr->setParent(token); // do not forget!
            token->getChildren().push_back(ptr); // modify last deepest open token
        }

        if(nextType == MarkdownTokenType::CODE) assert(0); // 暂不允许内嵌代码

        shared_ptr<MarkdownToken> next = scanToken(true, true); // 嵌套

        if(next){
            // if new token then push back
            // rely on different return statements:
            // - return ptr
            // - return nullptr
            next->setParent(token); // do not forget!
            token->getChildren().push_back(next);
        }

        return nullptr;
    }
}


shared_ptr<MarkdownToken> MarkdownScanner::orderedList(bool inner, bool list)
{
    assert(match(' ')); // 必须有一个空格

    shared_ptr<MarkdownToken> token = lastDeepestOpen();

    while(token){
        if(token->getTokenType() == MarkdownTokenType::OL)
            break;
        token = token->getParent();
    }

    if(!token){ // new token
        shared_ptr<MarkdownToken> ptr(
                    new MarkdownToken(
                        MarkdownTokenType::OL));

        shared_ptr<MarkdownToken> next = scanToken(true, true);
        next->setParent(ptr);
        ptr->getChildren().push_back(next);

        if(!inner)
            this->tokens.push_back(ptr);

        return ptr;
    }else{
        MarkdownTokenType nextType = advanceType();
        if(nextType == MarkdownTokenType::OL){ // 嵌套有序列表
            shared_ptr<MarkdownToken> ptr(
                        new MarkdownToken(
                            MarkdownTokenType::OL));
            ptr->setParent(token); // do not forget!
            token->getChildren().push_back(ptr); // modify last deepest open token
        }

        if(nextType == MarkdownTokenType::CODE) assert(0); // 暂不允许内嵌代码

        shared_ptr<MarkdownToken> next = scanToken(true, true); // 嵌套

        if(next){
            // if new token then push back
            // rely on different return statements:
            // - return ptr
            // - return nullptr
            next->setParent(token); // do not forget!
            token->getChildren().push_back(next);
        }

        return nullptr;
    }
}

shared_ptr<MarkdownToken> MarkdownScanner::scanToken(bool inner, bool list)
{
    char c = advance();
    switch (c) {
    case '#':{
        return heading(inner, list);
    }
    case '-':{
        return unorderedList(inner, list);
    }
    case '>':{
        return quote(inner, list);
    }
    case '1':{
        if(match('.')){
            return orderedList(inner, list);
        }
    }
    default:{
        current--;
        if(startwith(current, "    ")){
            current += 4;
            return code_space(inner, list);
        }
        if(startwith(current, "```\n")){
            current += 4;
            return code_fence(inner, list);
        }
        return paragraph(inner, list);
    }

    }
}

void MarkdownScanner::scanTokens()
{
    while(current < ori.length()){
        scanToken(false, false);
    }
}


