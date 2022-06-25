#pragma once

#include <QtWidgets/QMainWindow>
#include <QAbstractButton>

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

namespace pf {

class MainWnd : public QMainWindow
{
Q_OBJECT
public:
    MainWnd();
    virtual ~MainWnd();
    
protected slots:
    
protected:

};

}
