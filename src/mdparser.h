#ifndef MDPARSER_H
#define MDPARSER_H

#include "mdtoken.h"

using std::string;
using std::vector;
using std::shared_ptr;

class MarkdownParser
{
public:
    MarkdownParser(vector<shared_ptr<MarkdownToken>>);
    void start();
    string getRes() const;

private:
    void pre();
    void post();
    void iterate(const shared_ptr<MarkdownToken> &);
    void handleTokenPre(const shared_ptr<MarkdownToken> &);
    void handleTokenPost(const shared_ptr<MarkdownToken> &);
    void handleTokenIn(const shared_ptr<MarkdownToken> &);
    string handleText(const string &);
    string handleTextLine(const string &);

    string bold(const string &);
    string italic(const string &);
    string boldAndItalic(const string &);

    vector<shared_ptr<MarkdownToken>> tokens;
    string res;

    unsigned long long listDepth = 0;
    bool inQuote = false;
};

#endif // MDPARSER_H
