#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeView>
#include <QSyntaxHighlighter>

#include "highlighter.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void contextMenu(QModelIndex index);

private slots:
    void open();
    void close();

private:
    QTabWidget* tabWidget;
    QSyntaxHighlighter* syntaxHighlighter;


    void createActions();
    void createMenus();

private:
//    XmlModel* model;
    QTreeView* treeView;

    QMenu *fileMenu;
    QAction *openAct;
    QAction *closeAct;
    QAction *exitAct;
    QAction *makeActiveAct;

public slots:
    void about();
    void newFile();
    void openFile(const QString &path = QString());

private:
    void setupEditor();
    void setupFileMenu();
    void setupHelpMenu();

    QTextEdit *editor;
    Highlighter *highlighter;
};
#endif // MAINWINDOW_H
