#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    QWidget *widget = new QWidget;

    model=new XmlModel(this);
    treeView=new QTreeView(this);
    treeView->setModel(model);
    treeView->setContextMenuPolicy(Qt::ContextMenuPolicy());
    setCentralWidget(treeView);

    treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(contextMenu(QPoint)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);

    createActions();
    createMenus();

    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);

    setWindowTitle(tr("Menu"));
    setMinimumSize(160, 160);
    resize(480, 320);
}

MainWindow::~MainWindow(){
    delete model;
    delete treeView;

    delete fileMenu;
    delete openAct;
    delete closeAct;
    delete exitAct;
}

void MainWindow::contextMenu(QPoint index)
{
    if(model->getObjectByIndex(model->parent(treeView->indexAt(index)))==model->getRoot()){
        QMenu menu(this);
        model->setCurrentIndex(treeView->indexAt(index));
//        menu.addAction(makeActiveAct);
        menu.exec(QCursor::pos());
    }
}

void MainWindow::slotNew() //names!  &  does not work
{
    QStringList filename = {"New File"};
    model->addFiles(&filename);
    treeView->reset();
}

void MainWindow::slotOpen()
{
    QStringList FilesName = QFileDialog::getOpenFileNames(this,"Open XML-file","C:","XML-file(*.xml)");
    model->addFiles(&FilesName);
    treeView->reset();
}

void MainWindow::slotSave()
{

}

void MainWindow::slotSaveAs()
{

}

void MainWindow::slotSaveAll()
{

}

void MainWindow::slotClose()
{
    delete model;
    model=new XmlModel(this);
    treeView->setModel(model);
    treeView->reset();
}

void MainWindow::slotCloseAll()
{
    delete model;
    model=new XmlModel(this);
    treeView->setModel(model);
    treeView->reset();
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Make a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::slotNew);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::slotOpen);

    saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save active file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::slotSave);

    saveAsAct = new QAction(tr("SaveAs..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save active file as.."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::slotSaveAs);

    saveAllAct = new QAction(tr("SaveAll"), this);
    saveAllAct->setStatusTip(tr("Save all files"));
    connect(saveAllAct, &QAction::triggered, this, &MainWindow::slotSaveAll);

    closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcuts(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close this documents"));
    connect(closeAct, &QAction::triggered, this, &MainWindow::slotClose);

    closeAllAct = new QAction(tr("Close all"), this);
    closeAllAct->setStatusTip(tr("Close all documents"));
    connect(closeAllAct, &QAction::triggered, this, &MainWindow::slotCloseAll);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveAllAct);
    fileMenu->addAction(closeAct);
    fileMenu->addAction(closeAllAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}
