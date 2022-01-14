#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
{
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    model=new XmlModel(this);
    treeView=new QTreeView(this);
    treeView->setModel(model);
    treeView->setContextMenuPolicy(Qt::ContextMenuPolicy());
    treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(treeView);
    widget->setLayout(layout);

    createActions();
    createMenus();

    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);

    setWindowTitle(tr("Menu"));
    setMinimumSize(160, 160);
    resize(480, 320);

    setupFileMenu();
    setupHelpMenu();
    setupEditor();

    setCentralWidget(editor);
    setWindowTitle(tr("Syntax Highlighter"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Syntax Highlighter"),
                tr("<p>The <b>Syntax Highlighter</b> example shows how " \
                   "to perform simple syntax highlighting by subclassing " \
                   "the QSyntaxHighlighter class and describing " \
                   "highlighting rules using regular expressions.</p>"));
}

void MainWindow::newFile()
{
    editor->clear();
}

void MainWindow::openFile(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "C++ Files (*.cpp *.h)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
            editor->setPlainText(file.readAll());
    }
}

//! [1]
void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new QTextEdit;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());

    QFile file("mainwindow.h");
    if (file.open(QFile::ReadOnly | QFile::Text))
        editor->setPlainText(file.readAll());
}
//! [1]

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this,
                        &MainWindow::newFile, QKeySequence::New);
    fileMenu->addAction(tr("&Open..."),
                        this, [this](){ openFile(); }, QKeySequence::Open);
    fileMenu->addAction(tr("E&xit"), qApp,
                        &QApplication::quit, QKeySequence::Quit);
}

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

MainWindow::~MainWindow(){
    delete model;
    delete treeView;

    delete fileMenu;
    delete openAct;
    delete closeAct;
    delete exitAct;
    delete makeActiveAct;
}

void MainWindow::contextMenu(QModelIndex index)
{
    if(model->getObjectByIndex(model->parent(index))==model->getRoot()){
        QMenu menu(this);
        model->setCurrentIndex(index);
        menu.addAction(makeActiveAct);
        menu.exec(QCursor::pos());
    }
}

void MainWindow::open()
{
    QStringList FilesName = QFileDialog::getOpenFileNames(this,"Open XML-file","C:","XML-file(*.xml)");
    model->addFiles(FilesName);
    treeView->reset(); //Reset the internal state of the view.
}

void MainWindow::close()
{
    delete model;
    model=new XmlModel(this);
    treeView->setModel(model);
    treeView->reset();
}

void MainWindow::createActions()
{

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    closeAct = new QAction(tr("&Close all"), this);
    closeAct->setShortcuts(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close all documents"));
    connect(closeAct, &QAction::triggered, this, &MainWindow::close);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    makeActiveAct = new QAction(tr("Make Active"), this);
    makeActiveAct->setStatusTip(tr("Make active the current selection's contents"));
    connect(makeActiveAct, &QAction::triggered, model, &XmlModel::makeActiveObj);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(closeAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}



