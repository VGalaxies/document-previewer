#ifndef MDTOKEN_H
#define MDTOKEN_H

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::shared_ptr;

enum class MarkdownTokenType
{
    H1 = 1,
    H2 = 2,
    H3 = 3,
    H4 = 4,
    H5 = 5,
    H6 = 6,
    PARA,
    CODE,
    QUOTE,
    UL,
    OL,
};

class MarkdownToken
{
public:
    MarkdownToken(MarkdownTokenType type, string content, bool state);
    MarkdownToken(MarkdownTokenType type, string content);
    MarkdownToken(MarkdownTokenType type);

    MarkdownTokenType getTokenType() const;
    string getContent() const;
    bool getState() const;
    vector<shared_ptr<MarkdownToken>> & getChildren();
    shared_ptr<MarkdownToken> & getParent();

    void changeState();
    void appendContent(string str);
    void setParent(shared_ptr<MarkdownToken> &);

private:
    bool state = false; // false -> open
    vector<shared_ptr<MarkdownToken>> children;
    shared_ptr<MarkdownToken> parent;
    MarkdownTokenType type;
    string content;
};

#endif // MDTOKEN_H
