#include "settingdlg.h"
#include "utils/logger.h"
#include "utils/preference.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "utils/i18n.h"

using namespace pf;

SettingDlg::SettingDlg(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QWidget *settings = new QWidget();
    QFormLayout *container = new QFormLayout(settings);
    
    lang_ = new QComboBox();
    lang_->addItem("en");
    lang_->addItem("cn");
    std::string curr_lang = preference::i().get("lang", "cn");
    lang_->setCurrentIndex(lang_->findText(QString::fromStdString(curr_lang)));
    container->addRow(PF_TEXT("setting.lang", "Language"), lang_);

    comp_ = new QComboBox();
    comp_->addItem("GPU");
    comp_->addItem("CPU");
    std::string curr_compute = preference::i().get("comp", "GPU");
    comp_->setCurrentIndex(comp_->findText(QString::fromStdString(curr_compute)));
    container->addRow(PF_TEXT("setting.comp", "Compute Unit"), comp_);

    scale_ = new QComboBox();
    scale_->addItem("2x");
    scale_->addItem("3x");
    scale_->addItem("4x");
    std::string curr_scale = preference::i().get("scale", "4x");
    scale_->setCurrentIndex(scale_->findText(QString::fromStdString(curr_scale)));
    container->addRow(PF_TEXT("setting.scale", "SR Scale"), scale_);

    QHBoxLayout *buttonGroups = new QHBoxLayout();
    QPushButton *okay = new QPushButton();
    okay->setText(PF_TEXT("main.ok", "OK"));
    buttonGroups->addWidget(okay);
    connect(okay, &QPushButton::clicked, this, &SettingDlg::apply);

    QPushButton *cancel = new QPushButton();
    cancel->setText(PF_TEXT("main.cancel", "Cancel"));
    buttonGroups->addWidget(cancel);
    connect(cancel, &QPushButton::clicked, this, &SettingDlg::cancel);

    vLayout->addWidget(settings);
    vLayout->addLayout(buttonGroups);
}

SettingDlg::~SettingDlg() {

}

void SettingDlg::apply() {
    preference::i().set("lang", lang_->currentText().toStdString());
    preference::i().set("comp", comp_->currentText().toStdString());
    preference::i().set("scale", scale_->currentText().toStdString());
    preference::i().save();
    close();
}

void SettingDlg::cancel() {
    close();
}
