#include "mainwnd.h"

#include "utils/system.h"
#include "utils/logger.h"
#include "utils/preference.h"
#include "utils/i18n.h"

using namespace pf;

MainWnd::MainWnd()
    : QMainWindow(nullptr)
{
    int width = preference::i().get("main-width", 1280);
    int height = preference::i().get("main-height", 720);
    resize(width, height);

    setWindowTitle(PF_TEXT("main.title", "Picture Fun"));

    /*
     QWidget *centralwidget = new QWidget(this);
     splitter_ = new QSplitter(centralwidget);
     splitter_->setOrientation(Qt::Vertical);

     auto centralLayout = new QVBoxLayout(centralwidget);
     centralLayout->setSpacing(0);

     logView_ = new LogViewer();

     splitter_->setStyleSheet("QSplitter::handle { background-color: lightgray; margin-top: 3px;}");
     splitter_->setHandleWidth(8);
     splitter_->setContentsMargins(0, 0, 0, 0);

     QWidget *container = new QWidget();
     QHBoxLayout *containerLayout = new QHBoxLayout(container);

     QWidget *buttonGroup = new QWidget();
     QVBoxLayout *rightLayout = new QVBoxLayout(buttonGroup);
     buttonGroup->setMinimumWidth(240);

     QLabel *runningLabel = new QLabel();
     runningLabel->setText(LITERAL_DEFAULT("running", "Running Status:"));
     runningStatus_ = new InfoLabel();

     QHBoxLayout *runBox = new QHBoxLayout();
     runningStatus_->setText(LITERAL_DEFAULT("safety", "Safety"));
     runningStatus_->setSliderColorOff(QColor(255, 0, 0));
     runningStatus_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
     runBox->addWidget(runningStatus_);
     runBox->addSpacerItem(new QSpacerItem(100, 10, QSizePolicy::Maximum, QSizePolicy::Ignored));

     QGridLayout *connectGrid = new QGridLayout();
     for (int i = 0; i < 5; i++) {
         connectStatus_[i] = new InfoLabel();
         QString stext;
         stext.sprintf("%s%d", LITERAL_DEFAULT("camera", "Cam"), i + 1);
         connectStatus_[i]->setText(stext);
         connectGrid->addWidget(connectStatus_[i], i / 3, i % 3);
     }

     QPushButton *btnManual = new QPushButton();
     btnManual->setText(LITERAL_DEFAULT("manual", "Manual"));

     QPushButton *btnSetting = new QPushButton();
     btnSetting->setText(LITERAL_DEFAULT("setting", "Setting"));

     QPushButton *btnCapture = new QPushButton();
     btnCapture->setText(LITERAL_DEFAULT("capture", "Capture"));

     rightLayout->addWidget(runningLabel);
     rightLayout->addLayout(runBox);
     rightLayout->addLayout(connectGrid);
     rightLayout->addWidget(btnManual);
     rightLayout->addWidget(btnSetting);
     rightLayout->addWidget(btnCapture);
     rightLayout->addSpacerItem(new QSpacerItem(10, 50, QSizePolicy::Fixed, QSizePolicy::Expanding));

     resultViewCam1_ = new ImageView();
     resultViewCam1_->setMinimumHeight(360);
     resultViewCam1_->setScaledContents(true);
     resultViewCam1_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     resultViewCam1_->setViewOnly(true);

     resultViewCam2_ = new ImageView();
     resultViewCam2_->setMinimumHeight(360);
     resultViewCam2_->setScaledContents(true);
     resultViewCam2_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     resultViewCam2_->setViewOnly(true);

     resultViewCam3_ = new ImageView();
     resultViewCam3_->setMinimumHeight(360);
     resultViewCam3_->setScaledContents(true);
     resultViewCam3_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     resultViewCam3_->setViewOnly(true);

     resultViewCam4_ = new ImageView();
     resultViewCam4_->setMinimumHeight(360);
     resultViewCam4_->setScaledContents(true);
     resultViewCam4_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     resultViewCam4_->setViewOnly(true);

     resultViewOCR_ = new ImageView();
     resultViewOCR_->setMinimumHeight(360);
     resultViewOCR_->setScaledContents(true);
     resultViewOCR_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     resultViewOCR_->setViewOnly(true);

     QGridLayout *resultViewGrid = new QGridLayout();
     QGridLayout *detectionViewGrid = new QGridLayout();

     detectionViewGrid->addWidget(resultViewCam1_, 0 / 2, 0 % 2);
     detectionViewGrid->addWidget(resultViewCam2_, 1 / 2, 1 % 2);
     detectionViewGrid->addWidget(resultViewCam3_, 2 / 2, 2 % 2);
     detectionViewGrid->addWidget(resultViewCam4_, 3 / 2, 3 % 2);

     resultViewGrid->addLayout(detectionViewGrid, 0/2, 0%2);
     resultViewGrid->addWidget(resultViewOCR_, 1/2, 1%2);

     containerLayout->addLayout(resultViewGrid);

     containerLayout->addWidget(buttonGroup);

     splitter_->addWidget(container);
     splitter_->addWidget(logView_);

     centralLayout->addWidget(splitter_);
     setContentsMargins(0, 0, 0, 0);

     setCentralWidget(centralwidget);

     connect(btnManual, SIGNAL(clicked()), this, SLOT(onManual()));
     connect(btnSetting, SIGNAL(clicked()), this, SLOT(onSetting()));
     connect(btnCapture, SIGNAL(clicked()), this, SLOT(onCapture()));
 */
}

MainWnd::~MainWnd() {
    
}
