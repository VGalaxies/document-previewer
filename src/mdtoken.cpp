#include "mdtoken.h"

#include <string>

MarkdownToken::MarkdownToken(MarkdownTokenType type, string content, bool state)
{
    this->state = state;
    this->type = type;
    this->content = content;
}

MarkdownToken::MarkdownToken(MarkdownTokenType type, string content)
{
    this->type = type;
    this->content = content;
}

MarkdownToken::MarkdownToken(MarkdownTokenType type)
{
    this->type = type;
}


MarkdownTokenType MarkdownToken::getTokenType() const
{
    return type;
}

string MarkdownToken::getContent() const
{
    return content;
}

bool MarkdownToken::getState() const
{
    return state;
}

vector<shared_ptr<MarkdownToken>> & MarkdownToken::getChildren()
{
    return children;
}

shared_ptr<MarkdownToken> & MarkdownToken::getParent()
{
    return parent;
}

void MarkdownToken::changeState()
{
    state = true;
}

void MarkdownToken::appendContent(string str)
{
    content += str;
}

void MarkdownToken::setParent(shared_ptr<MarkdownToken> & parent)
{
    this->parent = parent;
}
