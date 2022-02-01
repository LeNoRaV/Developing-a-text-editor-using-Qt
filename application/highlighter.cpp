#include "highlighter.h"

MySyntaxHighlighter::MySyntaxHighlighter(const QString& nameFile,QTextDocument *parent,const QString nameFileSyntax)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    QTextCharFormat textCharFormat;

    QDomDocument domDocument;
    QFile file(nameFileSyntax);
    if (file.open(QIODevice::ReadOnly)){
        if(domDocument.setContent(&file)){
            QDomElement domElement=domDocument.documentElement();
            auto syntaxElements = domElement.elementsByTagName("syntax");
            if(!syntaxElements.isEmpty()){
                for(auto i=0;i<syntaxElements.count();++i){
                    if(syntaxElements.item(i).toElement().attribute("ext_list").contains(QFileInfo(nameFile).suffix()) && QFileInfo(nameFile).suffix()!=Q_NULLPTR){
                        auto basicElement=syntaxElements.item(i).toElement().elementsByTagName("basic");
                        auto ruleElements=basicElement.item(0).toElement().elementsByTagName("rule");
                        if(!ruleElements.isEmpty()){
                            for(int j=0;j<ruleElements.count();++j){
                                auto pattern=ruleElements.item(j).toElement().elementsByTagName("pattern");
                                auto format=ruleElements.item(j).toElement().elementsByTagName("format");
                                rule.pattern=QRegularExpression(pattern.item(0).toElement().attribute("value"));
                                rule.format.setForeground(QColor(format.item(0).toElement().attribute("foreground")));
                                highlightingRules.append(rule);
                            }
                        }
                        auto commentElement=syntaxElements.item(i).toElement().elementsByTagName("comment");
                        auto ruleElements2=commentElement.item(0).toElement().elementsByTagName("rule");
                        if(!ruleElements2.isEmpty()){
                            auto pattern=ruleElements2.item(0).toElement().elementsByTagName("pattern");
                            auto format=ruleElements2.item(0).toElement().elementsByTagName("format");
                            auto pattern2=ruleElements2.item(1).toElement().elementsByTagName("pattern");
                            multiLineCommentFormat.setForeground(QColor(format.item(0).toElement().attribute("foreground")));
                            commentStartExpression = QRegularExpression(pattern.item(0).toElement().attribute("value"));
                            commentEndExpression = QRegularExpression(pattern2.item(0).toElement().attribute("value"));
                        }
                    }
                }
            }
            else TextError=tr("Не получилось найти расцветки синтаксиса в файле");
        }
        else TextError=tr("Не получилось обработать файл с расцветками синтаксиса");
    }
    else TextError=tr("Не открылся файл с расцветками синтаксиса");
}

void MySyntaxHighlighter::highlightBlock(const QString &text)
{
    if(multiLineCommentFormat==QTextCharFormat() && commentStartExpression==QRegularExpression() && commentEndExpression==QRegularExpression())
        return;

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
