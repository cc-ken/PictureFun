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

namespace pf {

class MainWnd : public QMainWindow
{
Q_OBJECT
public:
    MainWnd(QApplication *app);
    virtual ~MainWnd();
    
protected slots:
    
protected:
    void createMenus();
    void createActions();
    
    QApplication *application_;
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
    QLabel *infoLabel;
    QToolBar *toolbar;
};

}
