#include "mainwnd.h"

#include "utils/system.h"
#include "utils/logger.h"
#include "utils/preference.h"
#include "utils/i18n.h"

#include <QStyle>
#include <QFileDialog>
#include <QIcon>
#include <QColorDialog>

#include "ncnn_engine.h"
#include "ppmatting.h"
#include "realesrgan.h"
#include "imgproc.h"

using namespace pf;

MainWnd::MainWnd(QApplication *app)
    : QMainWindow(nullptr)
    , application_(app)
    , bgColor_(120, 255, 155)
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
    
    NcnnEngine::instance().init();
}

MainWnd::~MainWnd()
{
    NcnnEngine::instance().uninit();
}

void MainWnd::pixelValue(int x, int y, int r, int g, int b)
{
#if !defined(NDEBUG)
    statusBar()->showMessage(QString::asprintf("[%d,%d] (r=%d,g=%d,b=%d)", x, y, r, g, b));
#endif
}

void MainWnd::createMenus()
{
    fileMenu = menuBar()->addMenu(PF_TEXT("main.file", "&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(PF_TEXT("main.edit", "&Edit"));
    editMenu->addAction(fitAct);
    editMenu->addAction(selectAct);
    editMenu->addAction(bgRemoveAct);
    editMenu->addAction(bgColorAct);

    helpMenu = menuBar()->addMenu(PF_TEXT("main.help", "&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

QIcon MainWnd::loadIcon(const char* iconName) {
    QString res = QString::asprintf(":/res/actions/%s/%s", (isDark() ? "white" : "black"), iconName);
    return QIcon(res);
}

void MainWnd::createActions()
{
    openAct = new QAction(loadIcon("open.ico"), PF_TEXT("main.open", "&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(PF_TEXT("tips.open", "Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWnd::open);
    toolbar->addAction(openAct);

    saveAct = new QAction(loadIcon("save.ico"), PF_TEXT("main.save", "&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(PF_TEXT("tips.save", "Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWnd::save);
    toolbar->addAction(saveAct);

    exitAct = new QAction(PF_TEXT("main.exit", "E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(PF_TEXT("tips.exit", "Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
    
    fitAct = new QAction(loadIcon("arrow_expand.ico"), PF_TEXT("main.fit", "&Fit"), this);
    fitAct->setStatusTip(PF_TEXT("tips.fit", "Fit the image size to the display window."));
    connect(fitAct, &QAction::triggered, this, &MainWnd::fit);
    toolbar->addAction(fitAct);

    selectAct = new QAction(loadIcon("select.ico"), PF_TEXT("main.select", "Select"), this);
    selectAct->setStatusTip(PF_TEXT("tips.select", "toggle select region and pan the image."));
    selectAct->setCheckable(true);
    selectAct->setChecked(false);
    //toolbar->addAction(selectAct);
    connect(selectAct, &QAction::triggered, this, &MainWnd::select);

    bgRemoveAct = new QAction(loadIcon("bg_remover.ico"), PF_TEXT("main.bg_remover", "BG Remove"), this);
    bgRemoveAct->setStatusTip(PF_TEXT("tips.bg_remover", "PPMatting to remove background of the picture."));
    toolbar->addAction(bgRemoveAct);
    connect(bgRemoveAct, &QAction::triggered, this, &MainWnd::bgRemove);

    bgColorAct = new QAction(loadIcon("pallete.ico"), PF_TEXT("main.pallete", "BG Color"), this);
    bgColorAct->setStatusTip(PF_TEXT("tips.pallete", "Change the background color."));
    toolbar->addAction(bgColorAct);
    connect(bgColorAct, &QAction::triggered, this, &MainWnd::bgColor);
    
    srAct = new QAction(loadIcon("zoom_in.ico"), PF_TEXT("main.sr", "Super Resolution"), this);
    srAct->setStatusTip(PF_TEXT("tips.sr", "Do super resolution with realESRGAN"));
    toolbar->addAction(srAct);
    connect(srAct, &QAction::triggered, this, &MainWnd::onSR);

    aboutAct = new QAction(PF_TEXT("main.about", "&About"), this);
    aboutAct->setStatusTip(PF_TEXT("main.about", "Show the application's About box"));
    //connect(aboutAct, &QAction::triggered, this, &MainWnd::about);
    
    aboutQtAct = new QAction(PF_TEXT("main.about_qt", "About &Qt"), this);
    aboutQtAct->setStatusTip(PF_TEXT("main.about_qt", "Show the Qt library's About box"));
    connect(aboutQtAct, &QAction::triggered, application_, &QApplication::aboutQt);
}

void MainWnd::open() {
    QString imgFile = QFileDialog::getOpenFileName(this, PF_TEXT("titles.open_file", "Open Image Files"), ".", tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg);;All Files (*.*)"));
    if (imgFile.isEmpty())
        return;
    
    imgView_->loadImageFile(imgFile);
}

void MainWnd::fit() {
    imgView_->fit();
}

void MainWnd::select() {
    imgView_->setSelecting(selectAct->isChecked());
}

void MainWnd::save() {
    QString imgFile = QFileDialog::getSaveFileName(this, PF_TEXT("titles.save_file", "Save Image Files"), ".", tr("Images (*.png *.jpg)"));
    if (imgFile.isEmpty())
        return;

    bool result = imgView_->save(imgFile);
    if (result) {
        auto resultString = QString(PF_TEXT("tips.saved", "file has been saved to ")) + imgFile;
        statusBar()->showMessage(resultString);
    }
}

void MainWnd::bgRemove() {
    PPMattingNCNN* matting = PPMattingNCNN::instance();
    cv::Mat input = imgView_->mat();
    if (input.empty()) {
        LOG_WARN("Please onpen picture to remove backgroound");
        return;
    }
    alpha_ = matting->inference(input);
    imgView_->setMatImage(blend(imgView_->origin(), alpha_, bgColor_));
}

void MainWnd::onSR() {
    RealESRGAN* sr = RealESRGAN::instance();
    cv::Mat input = imgView_->mat();
    if (input.empty()) {
        LOG_WARN("Please onpen picture to do super resolution");
        return;
    }
    sr->init(4);
    auto upscaled = sr->inference(input);
    imgView_->setMatImage(upscaled, false, true);
}

void MainWnd::bgColor()
{
    QColor color = QColorDialog::getColor(Qt::blue);
    bgColor_ = cv::Scalar(color.red(), color.green(), color.blue());
    LOG_INFO("MainWnd::bgColor, color=" << bgColor_);
    if (alpha_.empty() || alpha_.size() != imgView_->origin().size()) {
        return;
    }

    imgView_->setMatImage(blend(imgView_->origin(), alpha_, bgColor_));
}
