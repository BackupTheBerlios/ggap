#include "config.h"
#include "moo-macros.h"
#include "ggap/app.h"
#include "ggap/dialogs.h"
#include "ggap/wswindow.h"
#include "ggap/proc.h"
#include "ggap/utils.h"
#include "ggap/gap.h"
#include "ggap/prefsdialog.h"
#include "ggap/help.h"
#include "ggap/tester.h"
#include <qxtcommandoptions.h>
#include <QtGui>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

using namespace ggap;

namespace {

struct AppOptions {
    QStringList files_to_open;
    bool open_help_browser;
    QString help_url;
    bool raise;
    bool unit_tests;
    QStringList unit_test_args;

    AppOptions() :
        open_help_browser(false),
        raise(false),
        unit_tests(false)
    {
    }
};

struct AppData {
    AppOptions options;
    QString tempDir;
    QStringList tempFiles;
    QSettings *settings[2];

    bool inQuit;

    AppData()
    {
        settings[0] = settings[1] = 0;
        inQuit = false;
    }
};

static AppData appData;

}

static void usage(QxtCommandOptions &opts, bool show_qt)
{
    std::cout << "Application options:" << std::endl;
    opts.showUsage(show_qt);
    ::exit(EXIT_SUCCESS);
}

void App::parseOptions(int &argc, char **argv)
{
    QxtCommandOptions opts;
    opts.setFlagStyle(QxtCommandOptions::DoubleDash);

#ifdef MOO_ENABLE_UNIT_TESTS
    if (argc > 1 && strcmp(argv[1], "--ut") == 0)
    {
        appData.options.unit_tests = true;
        for (int i = 0; i < argc; ++i)
            if (i != 1)
                appData.options.unit_test_args << argv[i];
        return;
    }

    opts.add("--ut", "run unit tests");
#endif

#ifdef Q_OS_MAC
    opts.add("raise", "raise window");
#endif
    opts.add("help-url", "open url in help browser", QxtCommandOptions::ValueRequired);
    opts.add("help-browser", "open help browser");

    opts.add("version", "show version and exit");
    opts.add("help", "show this help text");
    opts.alias("help", "h");
    opts.add("help-all", "show also Qt options");

    opts.parse(argc, argv);

    if (opts.showUnrecognizedWarning())
        ::exit(EXIT_FAILURE);

    if (opts.count("help"))
        usage(opts, false);
    if (opts.count("help-all"))
        usage(opts, true);

    if (opts.count("version"))
    {
        std::cout << "ggap " GGAP_VERSION << std::endl;
        ::exit(EXIT_SUCCESS);
    }

#ifdef Q_OS_MAC
    appData.options.raise = opts.count("raise") != 0;
#endif

    appData.options.open_help_browser = opts.count("help-browser") != 0;
    if (opts.count("help-url"))
        appData.options.help_url = opts.value("help-url").toString();
    appData.options.files_to_open = opts.positional();
}

App::App(int &argc, char **argv) :
    QApplication(argc, argv)
{
    Q_INIT_RESOURCE(ggap);

#if !defined(Q_OS_MAC)
    QIcon icon;
    icon.addFile(":/icons/ggap/ggap-16.png", QSize(16, 16));
    icon.addFile(":/icons/ggap/ggap-32.png", QSize(32, 32));
    icon.addFile(":/icons/ggap/ggap-48.png", QSize(48, 48));
    icon.addFile(":/icons/ggap/ggap-64.png", QSize(64, 64));
    setWindowIcon(icon);
#endif

    setOrganizationName("ggap.sourceforge.net");
    setOrganizationDomain("ggap.sourceforge.net");
    setApplicationName("ggap");
#if QT_VERSION >= 0x040400
    setApplicationVersion(GGAP_VERSION);
#endif

#if defined(Q_OS_MAC)
    setAttribute(Qt::AA_DontShowIconsInMenus);
    initMacBundle();
#endif

#ifdef Q_OS_WIN32
    appData.settings[SettingsPrefs] = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                                    "ggap.sourceforge.net", "ggap", this);
    appData.settings[SettingsState] = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                                    "ggap.sourceforge.net", "ggap-state", this);
#else
    appData.settings[SettingsPrefs] = new QSettings("ggap.sourceforge.net", "ggap", this);
    appData.settings[SettingsState] = new QSettings("ggap.sourceforge.net", "ggap-state", this);
#endif

    qDebug() << "SettingsPrefs" << appData.settings[SettingsPrefs]->fileName();
    qDebug() << "SettingsState" << appData.settings[SettingsState]->fileName();

    setQuitOnLastWindowClosed(true);

    QDesktopServices::setUrlHandler("ghelp", this, "gapHelp");
    QDesktopServices::setUrlHandler("gfile", this, "gapHelp");
    QDesktopServices::setUrlHandler("help", this, "gapHelp");
}

#if defined(Q_OS_MAC)
static QString mac_to_qt(CFURLRef url)
{
    CFStringRef mac_path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    const char *c_path = CFStringGetCStringPtr(mac_path, CFStringGetSystemEncoding());
    QString path = c_path ? QString::fromUtf8(c_path) : QString("");
    CFRelease(mac_path);
    return path;
}

void App::initMacBundle()
{
//     QMenu *menu = new QMenu;
//     menu->addAction("lalala");
//     extern void qt_mac_set_dock_menu(QMenu *);
//     qt_mac_set_dock_menu(menu);

//     QSettings::setDefaultFormat(QSettings::IniFormat);

    CFBundleRef bundle = CFBundleGetMainBundle();

    if (!bundle)
        return;

    CFURLRef ggap_ini_url = CFBundleCopyResourceURL(bundle, CFSTR("ggap.ini"), 0, 0);
    if (!ggap_ini_url)
        return;

    qDebug() << mac_to_qt(ggap_ini_url);
    CFRelease(ggap_ini_url);

//     QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, mac_to_qt());

//     CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
//     CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
//     const char *pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
//     qDebug("Path = %s", pathPtr);
//     CFRelease(appUrlRef);
//     CFRelease(macPath);
}
#endif


QSettings *App::settings(SettingsType type)
{
    m_return_val_if_fail(type >= 0 && type < 2, new QSettings);
    return appData.settings[type];
}

static void deleteFile(const QString &name)
{
    QFileInfo fi(name);
    if (fi.isDir() && !fi.isSymLink())
    {
        QDir dir(name);
        QStringList files = dir.entryList();
        foreach (const QString &f, files)
            if (f != "." && f != "..")
                deleteFile(dir.filePath(f));
        QDir(fi.path()).rmdir(fi.fileName());
    }
    else
        QFile::remove(name);
}

App::~App()
{
    if (!appData.tempDir.isEmpty())
        deleteFile(appData.tempDir);
    foreach (const QString &name, appData.tempFiles)
        deleteFile(name);
}

void App::aboutDialog()
{
    AboutDialog::showDialog();
}

void App::prefsDialog()
{
    PrefsDialog::showDialog();
}

void App::gapHelp(const QUrl &url)
{
    HelpBrowser::showWindow(url);
}

static bool isRunning(App *app)
{
    bool running = false;
    QWidgetList list = app->topLevelWidgets();
    for (int i = 0; !running && i < list.size(); ++i)
    {
        QWidget *w = list.at(i);
        if (dynamic_cast<AppWindow*>(w))
            running = true;
    }
    return running;
}

void App::maybeQuit(QCloseEvent *e)
{
    if (e)
        e->ignore();

    if (appData.inQuit)
        return;

    appData.inQuit = true;
    closeAllWindows();
    appData.inQuit = false;

    if (e && !isRunning(this))
        e->accept();
}

void App::maybeQuit()
{
    maybeQuit(0);
}

void App::checkGap()
{
    static bool been_here;

    if (been_here)
        return;

    been_here = true;

    if (GapOptions().exe().isEmpty())
    {
        informationDialog(0, QString(), tr("GAP not found"),
                          tr("Please select the root GAP directory or GAP "
                             "executable in the Preferences dialog"));
        PrefsDialog::execDialog();
    }
}

int App::exec()
{
#ifdef MOO_ENABLE_UNIT_TESTS
    if (appData.options.unit_tests)
    {
        moo::test::Tester tester;
        return tester.exec(appData.options.unit_test_args);
    }
#endif

    checkGap();

    foreach (const QString &f, appData.options.files_to_open)
        loadFile(f);

    WsWindow::ensureWindow();

    if (appData.options.raise)
    {
        QWidget *win = 0;

        if (!(win = activeWindow()))
        {
            QWidgetList windows = topLevelWidgets();
            foreach (QWidget *w, windows)
                if ((win = dynamic_cast<WsWindow*>(w)))
                    break;
        }

        if (win)
        {
            win->raise();
            win->activateWindow();
        }
    }

    if (!appData.options.help_url.isEmpty())
        HelpBrowser::showWindow(appData.options.help_url);
    else if (appData.options.open_help_browser)
        HelpBrowser::showWindow();

    return QApplication::exec();
}

bool App::event(QEvent *e)
{
    switch (e->type())
    {
        case QEvent::FileOpen:
            loadFile(static_cast<QFileOpenEvent *>(e)->file());
            return true;

        case QEvent::Close:
            maybeQuit(static_cast<QCloseEvent*>(e));
            return true;

        default:
            return QApplication::event(e);
    }
}

void App::loadFile(const QString &filename)
{
    WsWindow::openFile(filename);
}


QString App::getUserDataDir(const QString &name)
{
#if QT_VERSION >= 0x040400
    QString dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#elif defined(Q_OS_UNIX)
    QString xdgDataHome = qgetenv("XDG_DATA_HOME");
    if (xdgDataHome.isEmpty())
        xdgDataHome = QDir::homePath() + QLatin1String("/.local/share");
    xdgDataHome += QString("/data/") + organizationName() + "/" + applicationName();
    QString dir = xdgDataHome;
#else
#error "Implement me"
#endif
    if (name.isEmpty())
        return dir;
    else
        return dir + "/" + name;
}

QString App::getUserDataFile(const QString &name)
{
    return getUserDataDir(name);
}

bool App::makeUserDataDir(const QString &name)
{
    QString path = getUserDataDir(name);
    bool ret = util::mkpath(path);
    if (!ret)
        qCritical() << "could not create directory '" << path << "'";
    return ret;
}

QString App::topTempDir()
{
    static QMutex mutex;

    QMutexLocker lock(&mutex);

    if (appData.tempDir.isEmpty())
    {
        QDir td = QDir::temp();
        qsrand(QDateTime::currentDateTime().toTime_t());
        for (int i = 0; i < 1000; ++i)
        {
            QString name;
            name.sprintf("-%08x", (quint32) qrand());
            name = applicationName() + name;
            if (td.mkdir(name))
            {
                appData.tempDir = td.filePath(name);
                qDebug() << "created temporary directory" << appData.tempDir;
                break;
            }
        }
        if (appData.tempDir.isEmpty())
        {
            // XXX ???
            qCritical("%s: oops", Q_FUNC_INFO);
        }
    }

    return appData.tempDir;
}

QString App::fileNameInTempDir(const QString &name)
{
    return name.isEmpty() ? topTempDir() : topTempDir() + "/" + name;
}

QString App::tempFileName(const QString &name)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    static QMap<QString, int> counter;
    QString fullname;

    if (name.isEmpty())
    {
        fullname = QString::number(++counter[""]);
    }
    else if (counter.contains(name))
    {
        fullname = QString("%1-%2").arg(++counter[name]).arg(name);
    }
    else
    {
        fullname = name;
        counter[name] = 0;
    }

    return fileNameInTempDir(fullname);
}

QDir App::makeTempDir()
{
    QDir d(tempFileName());
    d.mkpath(".");
    return d;
}

QString App::saveTempFile(const QString &contents, QString &error, const QString &name)
{
    QFile file;
    if (!openTempFile(file, error, name))
        return false;
    QByteArray bytes = contents.toUtf8();
    if (file.write(bytes) != bytes.size() || !file.flush())
    {
        error = file.errorString();
        file.remove();
        return QString();
    }
    file.close();
    return file.fileName();
}

bool App::openTempFile(QFile &file, QString &error, const QString &name)
{
    m_return_val_if_fail(!file.isOpen(), false);
    QString filename = tempFileName(name);
    file.setFileName(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        error = file.errorString();
        return false;
    }
    return true;
}
