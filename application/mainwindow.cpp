#include <QtWidgets>

#include "mainwindow.h"
#include "exit.h"

MainWindow::MainWindow()
    : textEdit(new QPlainTextEdit)
{
    tabWidget=new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMinimumHeight(500);
    tabWidget->setMinimumWidth(1000);
    tabWidget->setTabShape(QTabWidget::Triangular);

    setCentralWidget(tabWidget);

    createFile();
    createEdit();
    createView();
    createStatusBar();

    readSettings();

    connect(tabWidget,SIGNAL(tabCloseRequested(int)),SLOT(slotTabCloseRequested(int)));

#ifndef QT_NO_SESSIONMANAGER
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

//    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
    setWindowTitle(tr("Text editor"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QStringList listNoSaveFiles;
    int count=tabWidget->count();
    for(int i=0;i<count;++i){
        tabWidget->setCurrentIndex(i);
        if(tabWidget->currentWidget()->property("PathAndName")==QVariant())
            listNoSaveFiles.push_back(tabWidget->tabText(tabWidget->currentIndex()));
        else{
            QFile file(tabWidget->currentWidget()->property("PathAndName").toString());
            if(file.open(QIODevice::ReadOnly)){
                QString text=file.readAll();
                QTextEdit* page=static_cast<QTextEdit*>(tabWidget->currentWidget());
                if(page->toPlainText()!=text) listNoSaveFiles.push_back(tabWidget->currentWidget()->property("PathAndName").toString());
            }
            else getWindowForTextErrors(tr("Can't open file ")+tabWidget->tabText(tabWidget->currentIndex()));
        }
    }
    if(listNoSaveFiles.count()!=0){
        ExitWindow* exitWindow=new ExitWindow(&listNoSaveFiles);
        int resultExitWindow=exitWindow->exec();
        if(resultExitWindow==1){
            QWidget* widget=tabWidget->currentWidget();
            for(int a=0;a<tabWidget->count();++a){
                tabWidget->setCurrentIndex(a);
                save();
            }
            tabWidget->setCurrentWidget(widget);
        }
        if(resultExitWindow==1 || resultExitWindow==2) event->accept();
        else event->ignore();
    }
}

void MainWindow::newFile()
{
    ++numberNewFiles;
    QTextEdit* newWidget=new QTextEdit();
    tabWidget->addTab(newWidget,tr("New file ")+QString::number(numberNewFiles));
    UpdatingOpenDocuments();

    connect(newWidget,SIGNAL(textChanged()),this,SLOT(slotTextChanged()));
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}


void MainWindow::saveAll()
{   
    QWidget* widget=tabWidget->currentWidget();
    for(int a=0;a<tabWidget->count();++a){
        tabWidget->setCurrentIndex(a);
        save();
    }
    tabWidget->setCurrentWidget(widget);
}

bool MainWindow::close()
{
    bool close=true;
    if(tabWidget->currentWidget()->property("PathAndName")==QVariant()){
        close=maybeSave();
    }
    else{
        QFile file(tabWidget->currentWidget()->property("PathAndName").toString());
        if(file.open(QIODevice::ReadOnly)){
            QString text=file.readAll();
            QTextEdit* page=static_cast<QTextEdit*>(tabWidget->currentWidget());
            if(page->toPlainText()!=text) close=maybeSave();
        }
        else getWindowForTextErrors(tr("File can't be open ")+tabWidget->tabText(tabWidget->currentIndex()));
    }
    if(close){
        tabWidget->currentWidget()->close();
        tabWidget->removeTab(tabWidget->currentIndex());
    }
    UpdatingOpenDocuments();
    return close;
}

void MainWindow::closeAll()
{
    int count=tabWidget->count();
    for(int a=0;a<count;++a){
        tabWidget->setCurrentIndex(0);
        if(!close()) return;
    }
}

void MainWindow::createFile()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    QAction *saveAllAct = new QAction(tr("Save All"), this);
    saveAllAct->setStatusTip(tr("Save all documents to disk"));
    connect(saveAllAct, &QAction::triggered, this, &MainWindow::saveAll);
    fileMenu->addAction(saveAllAct);

    QAction *closeAct = new QAction(tr("Close"), this);
    closeAct->setStatusTip(tr("Close the document"));
    connect(closeAct, &QAction::triggered, this, &MainWindow::close);
    fileMenu->addAction(closeAct);

    QAction *closeAllAct = new QAction(tr("Close All"), this);
    closeAllAct->setStatusTip(tr("Close all documents"));
    connect(closeAllAct, &QAction::triggered, this, &MainWindow::closeAll);
    fileMenu->addAction(closeAllAct);

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
}

void MainWindow::createEdit()
{
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    QAction *deleteAct = new QAction(tr("&Delete"), this);
    deleteAct->setShortcuts(QKeySequence::Delete);
    deleteAct->setStatusTip(tr("Delete the clipboard's content"));
    editMenu->addAction(deleteAct);

    QAction *selectAllAct = new QAction(tr("&Select All"), this);
    selectAllAct->setStatusTip(tr("Select all clipboard's content"));
    editMenu->addAction(selectAllAct);

    connect(editMenu,SIGNAL(triggered(QAction*)),this,SLOT(slotMenuEdit(QAction*)));

    menuBar()->addSeparator();
}

void MainWindow::slotMenuEdit(QAction* action){
    if(action->text()==tr("Cu&t")){
        QTextEdit* text=(QTextEdit*)(tabWidget->currentWidget());
        text->cut();
    }
    if(action->text()==tr("&Copy")){
        QTextEdit* text=(QTextEdit*)(tabWidget->currentWidget());
        text->copy();
    }
    if(action->text()==tr("&Paste")){
        QTextEdit* text=(QTextEdit*)(tabWidget->currentWidget());
        text->paste();
    }
    if(action->text()==tr("&Delete")){
        QTextEdit* text=(QTextEdit*)(tabWidget->currentWidget());
        text->clear();
    }
    if(action->text()==tr("&Select All")){
        QTextEdit* text=(QTextEdit*)(tabWidget->currentWidget());
        text->selectAll();
    }
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::maybeSave()
{

    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    bool openingFile=false;
    for(int i=0;i<tabWidget->count();++i){
        if(tabWidget->tabText(i)==fileName) openingFile=true;
    }
    if(!openingFile){
        QString textFile=file.readAll();
        QTextEdit* newWidget=new QTextEdit();
        newWidget->setProperty("PathAndName",fileName);
        newWidget->setText(textFile);

        Highlighter* syntax=new Highlighter(fileName,newWidget->document());

        QFileInfo fileInfo(fileName);
        tabWidget->addTab(newWidget,fileInfo.fileName());

        connect(newWidget,SIGNAL(textChanged()),this,SLOT(slotTextChanged()));
        tabWidget->setCurrentWidget(newWidget);
    }
    else getWindowForTextErrors("File is already opened");
    UpdatingOpenDocuments();

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QString errorMessage;
    QTextEdit* page=static_cast<QTextEdit*>(tabWidget->currentWidget());

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << page->toPlainText();
        QFileInfo fileInfo(fileName);
        tabWidget->setTabText(tabWidget->currentIndex(),fileInfo.fileName());
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

//    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        if (textEdit->document()->isModified())
            save();
    }
}
#endif

void MainWindow::getWindowForTextErrors(const QString textError){
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(textError);
    msgBox.exec();
}

void MainWindow::slotTabCloseRequested(int index){
    QWidget* wgt=tabWidget->currentWidget();
    tabWidget->setCurrentIndex(index);
    close();
    tabWidget->setCurrentWidget(wgt);
}

void MainWindow::slotTextChanged(){
    if(tabWidget->tabText(tabWidget->currentIndex()).at(0)!='*'){
        QString str =tabWidget->tabText(tabWidget->currentIndex());
        tabWidget->setTabText(tabWidget->currentIndex(),"*"+str);
    }
    UpdatingOpenDocuments();
}

void MainWindow::UpdatingOpenDocuments(){
    if(stringListModel!=Q_NULLPTR){
        delete stringListModel;
        stringListModel=new QStringListModel(this);
        QStringList list;
        for(int i=0;i<tabWidget->count();++i){
            list.push_back(tabWidget->tabText(i));
        }
        stringListModel->setStringList(list);
        listView->setModel(stringListModel);
        listView->reset();
    }
}

void MainWindow::createView()
{

    QMenu* menuView=new QMenu(tr("&View"),this);
    menuView->addAction(tr("Show explorer"));
    menuView->addAction(tr("Show open document browser"));
    menuBar()->addMenu(menuView);

    connect(menuView,SIGNAL(triggered(QAction*)),this,SLOT(slotMenuView(QAction*)));
}

void MainWindow::slotMenuView(QAction* action){
    if(action->text()==tr("Show explorer")){
        fileSystemModel = new QFileSystemModel(this);
        fileSystemModel->setRootPath(QDir::currentPath());
        QDockWidget* dockWidget=new QDockWidget(tr("Explorer"),this);
        dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
        addDockWidget(Qt::LeftDockWidgetArea,dockWidget);
        QTreeView *treeView = new QTreeView(this);
        treeView->setModel(fileSystemModel);
        treeView->setMinimumWidth(150);
        dockWidget->setWidget(treeView);

        connect(treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(slotOpenFile(QModelIndex)));
    }
    if(action->text()==tr("Show open document browser")){
        stringListModel=new QStringListModel(this);
        QStringList list;
        for(int i=0;i<tabWidget->count();++i){
            list.push_back(tabWidget->tabText(i));
        }
        stringListModel->setStringList(list);
        listView=new QListView(this);
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setModel(stringListModel);
        listView->setMinimumWidth(100);
        QDockWidget* dockWidget=new QDockWidget(tr("Open documents"),this);
        dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
        addDockWidget(Qt::RightDockWidgetArea,dockWidget);
        dockWidget->setWidget(listView);

        connect(listView,SIGNAL(clicked(QModelIndex)),this,SLOT(slotActiveTab(QModelIndex)));
    }
}

void MainWindow::slotActiveTab(QModelIndex index){
    tabWidget->setCurrentIndex(index.row());
}

void MainWindow::slotOpenFile(QModelIndex index)
{
    QFile file(fileSystemModel->filePath(index));
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileSystemModel->filePath(index)), file.errorString()));
        return;
    }

    bool openingFile=false;
    for(int i=0;i<tabWidget->count();++i){
        if(tabWidget->tabText(i)==fileSystemModel->filePath(index)) openingFile=true;
    }
    if(!openingFile){
        QString textFile=file.readAll();
        QTextEdit* newWidget=new QTextEdit();
        newWidget->setProperty("PathAndName",fileSystemModel->filePath(index));
        newWidget->setText(textFile);

        QString str=fileSystemModel->fileName(index);
        Highlighter* syntax=new Highlighter(str,newWidget->document());

        QFileInfo fileInfo(fileSystemModel->filePath(index));
        tabWidget->addTab(newWidget,fileInfo.fileName());

        connect(newWidget,SIGNAL(textChanged()),this,SLOT(slotTextChanged()));
        tabWidget->setCurrentWidget(newWidget);
    }
    else getWindowForTextErrors("File is already opened");

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
}
