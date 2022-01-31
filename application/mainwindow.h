#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QDomDocument>
#include "highlighter.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTabWidget* tabWidget;
    QFileSystemModel *fileSystemModel=Q_NULLPTR;
    QStringListModel* stringListModel=Q_NULLPTR;
    QListView* listView;
    int numberNewFiles=0;

public:
    MainWindow();

    void loadFile(const QString &fileName);
    void getWindowForTextErrors(const QString);
    void UpdatingOpenDocuments();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void saveAll();
    bool close();
    void closeAll();
//    void documentWasModified();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif
    void slotTabCloseRequested(int);
    void slotTextChanged();
    void slotOpenFile(QModelIndex);
    void slotActiveTab(QModelIndex);
    void slotMenuEdit(QAction*);
    void slotMenuView(QAction*);

//    void createDockWidgets();
//    void loadCurrentFile(QModelIndex index);
//    void currentTab(QModelIndex index);

private:
    void createFile();
    void createEdit();
    void createView();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QString curFile; 
};

#endif
