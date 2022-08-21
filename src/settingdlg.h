#pragma once

#include <QDialog>
#include <QComboBox>

namespace pf {

class SettingDlg : public QDialog
{
Q_OBJECT
public:
    SettingDlg(QWidget *parent);
    virtual ~SettingDlg();

protected:
    QComboBox *lang_;
    QComboBox *comp_;
    QComboBox *scale_;
    
protected slots:
    void apply();
    void cancel();
};

}
