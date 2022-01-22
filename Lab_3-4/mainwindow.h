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
#include "xmlmodel.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private slots:
    void contextMenu(QPoint index);

private slots:
    void slotNew();
    void slotOpen();
    void slotSave();
    void slotSaveAs();
    void slotSaveAll();
    void slotClose();
    void slotCloseAll();

private:
    QTabWidget* tabWidget;
    QSyntaxHighlighter* syntaxHighlighter;


    void createActions();
    void createMenus();

private:
    XmlModel* model;
    QTreeView* treeView;

    QMenu *fileMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *saveAllAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *exitAct;

private:
    QTextEdit *editor;
    Highlighter *highlighter;
};
#endif // MAINWINDOW_H
