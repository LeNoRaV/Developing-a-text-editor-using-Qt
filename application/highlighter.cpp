#include "highlighter.h"

Highlighter::Highlighter(const QString &suffix, QTextDocument *parent, const QString &style_filename)
    : QSyntaxHighlighter(parent)
{
    supported = true;
    multiLineCommentFormat.setForeground(QColor(255, 198, 136));

    HighlightingRule rule;
    QTextCharFormat textCharFormat;
    QDomDocument domDocument;
    QFile file(style_filename);

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (file.open(QIODevice::ReadOnly)){
        if (domDocument.setContent(&file, &errorStr, &errorLine, &errorColumn)){
            QDomElement root = domDocument.documentElement();
            auto syntaxNodes = root.elementsByTagName("syntax");

            if (!syntaxNodes.isEmpty())
                for(auto i = 0; i < syntaxNodes.count(); ++i) {
                    QStringList ext_list = syntaxNodes.item(i).toElement().attribute("ext_list").split(QRegularExpression("\\s+"));
                    if ((ext_list.contains(suffix)) && (suffix != Q_NULLPTR)){
                        auto ruleNodes = syntaxNodes.item(i).toElement().elementsByTagName("rule");

                        if (!ruleNodes.isEmpty())
                            for(int j = 0; j < ruleNodes.count(); ++j){
                                auto pattern = ruleNodes.item(j).toElement().elementsByTagName("pattern");
                                auto format = ruleNodes.item(j).toElement().elementsByTagName("format");

                                rule.pattern = QRegularExpression(pattern.item(0).toElement().attribute("value"));
                                rule.format.setForeground(QColor(format.item(0).toElement().attribute("foreground")));
                                rule.format.setFontWeight(format.at(0).toElement().attribute("font_weight").toInt());

                                highlightingRules.append(rule);
                            }
                        auto startElem = root.elementsByTagName("startComment").item(0).toElement();
                        auto endElem = root.elementsByTagName("startComment").item(0).toElement();

                        QString startCommentNode = startElem.elementsByTagName("pattern").at(0).toElement().attribute("value");
                        QString endCommentNode = endElem.elementsByTagName("pattern").at(0).toElement().attribute("value");

                        commentStartExpression = QRegularExpression(startCommentNode);
                        commentEndExpression = QRegularExpression(endCommentNode);
                    }
                    else
                        supported = false;
                }
        }
        else qDebug() << errorStr
                      << "Wrong line: "
                      << errorLine
                      << "Wrong column: "
                      << errorColumn;
    }
    else qDebug() << "Can't open Xml-file";
}

bool Highlighter::isSupported()
{
    return supported;
}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex+ match.capturedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
