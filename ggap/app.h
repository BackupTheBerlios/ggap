#ifndef GGAP_APP_H
#define GGAP_APP_H

#include <QDir>
#include <QUrl>
#include <QStringList>
#include <QApplication>
#include <ggap/prefs.h>

class QFile;
class QSettings;
class QCloseEvent;

namespace ggap {

class App : public QApplication {
    Q_OBJECT

public:
    App(int &argc, char **argv);
    ~App();
    int exec ();

    static void parseOptions(int &argc, char **argv);

    bool makeUserDataDir(const QString &name = QString());
    QString getUserDataDir(const QString &name = QString());
    QString getUserDataFile(const QString &name);
    QString saveTempFile(const QString &contents, QString &error, const QString &name = QString());
    bool openTempFile(QFile &file, QString &error, const QString &name = QString());
    QString tempFileName(const QString &name = QString());
    QString fileNameInTempDir(const QString &name);
    QDir makeTempDir();

    QSettings *settings(SettingsType type);

    void allWindowsClosed();

    void deleteLater(QWidget *window);
private Q_SLOTS:
    void queuedDelete();

public Q_SLOTS:
    void maybeQuit();
    void aboutDialog();
    void prefsDialog();
    void gapHelp(const QUrl &url = QUrl());

private:
#ifdef Q_OS_MAC
    void initMacBundle();
#endif
    void checkGap();
    bool event(QEvent *e);
    void maybeQuit(QCloseEvent *e);
    void loadFile(const QString &filename);
    QString topTempDir();
};

#define gapApp (static_cast<ggap::App*>(QCoreApplication::instance()))

} // namespace ggap

#endif // GGAP_APP_H
