#ifndef MYSYNTAXHIGHLIGHTER_H
#define MYSYNTAXHIGHLIGHTER_H

#include "mainwindow.h"

class MySyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    MySyntaxHighlighter(const QString& nameFile,
                        QTextDocument *parent = 0,
                        const QString nameFileSyntax=":/xml-file.xml");

    QString TextError=Q_NULLPTR;

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat multiLineCommentFormat=QTextCharFormat();
    QRegularExpression commentStartExpression=QRegularExpression();
    QRegularExpression commentEndExpression=QRegularExpression();
};


#endif // MYSYNTAXHIGHLIGHTER_H
