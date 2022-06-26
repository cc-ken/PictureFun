#include "mainwnd.h"

#include "utils/system.h"
#include "utils/logger.h"
#include "utils/preference.h"
#include "utils/i18n.h"

#include <QStyle>
#include <QFileDialog>

using namespace pf;

MainWnd::MainWnd(QApplication *app)
    : QMainWindow(nullptr)
    , application_(app)
{
    int defWidth = 1280, defHeight = 720;
    getScreenResolution(defWidth, defHeight);
    int width = preference::i().get("main-width", int(defWidth * 0.75));
    int height = preference::i().get("main-height", int(defHeight * 0.75));
    resize(width, height);
    setMinimumSize(QSize(640, 480));

    setWindowTitle(PF_TEXT("main.title", "Picture Fun"));
    
    QWidget *widget = new QWidget;
    widget->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(widget);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(3);
    layout->setContentsMargins(3, 3, 3, 3);
    widget->setLayout(layout);

    toolbar = new QToolBar();
    //toolbar->setContentsMargins(5, 5, 5, 5);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    createActions();
    createMenus();
    
    QWidget *main = new QWidget();
    main->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    imgView_ = new ImageView(this);
    //imgView_->setStyleSheet(tr("border:2px solid darkgray"));
    imgView_->setContentsMargins(0, 0, 0, 5);
    QHBoxLayout *mainLayout = new QHBoxLayout(main);
    mainLayout->setContentsMargins(2, 0, 2, 0);
    mainLayout->addWidget(imgView_);
    connect(imgView_, &ImageView::selectPixel, this, &MainWnd::pixelValue);
    
    layout->addWidget(toolbar);
    layout->addWidget(main);
    
    statusBar()->showMessage(PF_TEXT("main.title", "Picture Fun"));
}

void MainWnd::pixelValue(int x, int y, int r, int g, int b)
{
    statusBar()->showMessage(QString::asprintf("[%d,%d] (r=%d,g=%d,b=%d)", x, y, r, g, b));
}

void MainWnd::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(fitAct);
    editMenu->addAction(selectAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWnd::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    //connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    openAct->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    connect(openAct, &QAction::triggered, this, &MainWnd::open);
    toolbar->addAction(openAct);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    saveAct->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    //connect(saveAct, &QAction::triggered, this, &MainWnd::save);
    toolbar->addAction(saveAct);

    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setStatusTip(tr("Print the document"));
    //connect(printAct, &QAction::triggered, this, &MainWnd::print);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    //connect(undoAct, &QAction::triggered, this, &MainWnd::undo);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    //connect(redoAct, &QAction::triggered, this, &MainWnd::redo);

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    //connect(cutAct, &QAction::triggered, this, &MainWnd::cut);

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    //connect(copyAct, &QAction::triggered, this, &MainWnd::copy);

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    //connect(pasteAct, &QAction::triggered, this, &MainWnd::paste);
    
    fitAct = new QAction(tr("&Fit"), this);
    fitAct->setStatusTip(tr("Fit the image size to the display window."));
    connect(fitAct, &QAction::triggered, this, &MainWnd::fit);
    fitAct->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogResetButton));
    toolbar->addAction(fitAct);

    selectAct = new QAction(tr("Select"), this);
    selectAct->setStatusTip(tr("toggle select region and pan the image."));
    selectAct->setCheckable(true);
    selectAct->setChecked(false);
    selectAct->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon));
    toolbar->addAction(selectAct);
    connect(selectAct, &QAction::triggered, this, &MainWnd::select);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    //connect(aboutAct, &QAction::triggered, this, &MainWnd::about);
    
    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, &QAction::triggered, application_, &QApplication::aboutQt);
}

void MainWnd::open() {
    QString imgFile = QFileDialog::getOpenFileName(this, "Open Image Files", ".", tr("Images (*.png *.xpm *.jpg *.bmp);;All Files (*.*)"));
    if (imgFile.isEmpty())
        return;
    
    imgView_->LoadImageFile(imgFile);
}

void MainWnd::fit() {
    imgView_->fit();
}

void MainWnd::select() {
    imgView_->setSelecting(selectAct->isChecked());
}
