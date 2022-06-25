#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QMessageBox>
#include <QtPlugin>

#include "mainwnd.h"

#include "utils/logger.h"
#include "utils/i18n.h"
#include "utils/system.h"

Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)

using namespace pf;

int main(int argc, char *argv[])
{
    Logger::i().init();
    auto textPath = getCurrFilePath() + "/lang/cn.xml";
    LOG_INFO("Loading texts from:" << textPath);
    i18n::i().load(textPath);
    
    QApplication app(argc, argv);
    
    MainWnd mainwnd;
    mainwnd.show();
    
    return app.exec();
}
