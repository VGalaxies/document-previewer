#ifndef MDSCANNER_H
#define MDSCANNER_H

#include "mdtoken.h"

using std::string;
using std::vector;
using std::shared_ptr;

class MarkdownScanner
{
public:
    MarkdownScanner(string);
    vector<shared_ptr<MarkdownToken>> getTokens() const;
    void scanTokens();

private:
    bool match(char);
    char advance();
    MarkdownTokenType advanceType();
    shared_ptr<MarkdownToken> scanToken(bool inner, bool list);

    shared_ptr<MarkdownToken> heading(bool inner, bool list);
    shared_ptr<MarkdownToken> paragraph(bool inner, bool list);
    shared_ptr<MarkdownToken> code_space(bool inner, bool list);
    shared_ptr<MarkdownToken> code_fence(bool inner, bool list);
    shared_ptr<MarkdownToken> quote(bool inner, bool list);
    shared_ptr<MarkdownToken> unorderedList(bool inner, bool list);
    shared_ptr<MarkdownToken> orderedList(bool inner, bool list);

    bool startwith(unsigned long long, string);

    shared_ptr<MarkdownToken> lastDeepestOpen();

    vector<shared_ptr<MarkdownToken>> tokens;
    string ori;
    unsigned long long current = 0;
};

#endif // MDSCANNER_H
