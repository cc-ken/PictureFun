#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QMessageBox>
#include <QtPlugin>

#include "mainwnd.h"

#include "utils/logger.h"
#include "utils/i18n.h"
#include "utils/preference.h"
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
    int target = Logger::eLogFile;
#if defined(_WINDOWS)
    target |= Logger::eLogMSVC;
#else
    target |= Logger::eLogSys;
#endif
    Logger::i().init(target);
    auto lang = preference::i().get("lang", "cn");
    auto textPath = getCurrFilePath() + "/lang/" + lang + ".xml";
    LOG_INFO("Loading texts from:" << textPath);
    i18n::i().load(textPath);

    QApplication app(argc, argv);
    
    Q_INIT_RESOURCE(icons);
    MainWnd mainwnd(&app);
    mainwnd.show();
        
    return app.exec();
}
