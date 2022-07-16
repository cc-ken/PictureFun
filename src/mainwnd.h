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
    virtual ~MainWnd() = default;
    
protected slots:
    void open();
    void fit();
    void select();
    void save();
    void pixelValue(int x, int y, int r, int g, int b);
    
protected:
    void createMenus();
    void createActions();
    
    QApplication *application_;
    ImageView *imgView_;
    
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *fitAct;
    QAction *selectAct;
    QLabel *infoLabel;
    QToolBar *toolbar;
};

}
