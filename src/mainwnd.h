#pragma once

#include <QtWidgets/QMainWindow>
#include <QAbstractButton>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QListView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QApplication>
#include <QToolBar>

#include "imageview.h"

namespace pf {

class MainWnd : public QMainWindow
{
Q_OBJECT
public:
    MainWnd(QApplication *app);
    virtual ~MainWnd();
    
protected slots:
    void open();
    void fit();
    void select();
    void save();
    void bgRemove();
    void bgColor();
    void onSR();
    void pixelValue(int x, int y, int r, int g, int b);
    void about();
    void setting();
    void closeEvent(QCloseEvent *event) override;
    
protected:
    void createMenus();
    void createActions();
    QIcon loadIcon(const char* iconName);
    
    QApplication *application_;
    ImageView *imgView_;
    
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *fitAct;
    QAction *selectAct;
    QAction *bgRemoveAct;
    QAction *bgColorAct;
    QAction *srAct;
    QAction *settingAct;

    QLabel *infoLabel;
    QToolBar *toolbar;
    cv::Scalar bgColor_;
    cv::Mat alpha_;
};

}
