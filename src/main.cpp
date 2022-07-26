#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QMessageBox>
#include <QtPlugin>

#include "mainwnd.h"

#include "utils/logger.h"
#include "utils/i18n.h"
#include "utils/system.h"

#if defined(_WINDOWS)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#elif defined(__APPLE__)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#endif
Q_IMPORT_PLUGIN(QICOPlugin)

using namespace pf;

int main(int argc, char *argv[])
{
    Logger::i().init();
    auto textPath = getCurrFilePath() + "/lang/cn.xml";
    LOG_INFO("Loading texts from:" << textPath);
    i18n::i().load(textPath);

    QApplication app(argc, argv);
    
    Q_INIT_RESOURCE(icons);
    MainWnd mainwnd(&app);
    mainwnd.show();
        
    return app.exec();
}
