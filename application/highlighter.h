#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

struct HighlightingRule
{
    QRegularExpression pattern;
    QTextCharFormat format;
};

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(const QString &suffix,
                QTextDocument *parent=0,
                const QString &style_filename=":/MyFiles/xml-file.xml");
    bool isSupported();

protected:
    void highlightBlock(const QString &text) override;

private:
    QVector<HighlightingRule> highlightingRules;
    bool supported;

    QTextCharFormat multiLineCommentFormat;
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
};

#endif // HIGHLIGHTER_H
