#include "ggap/utils.h"
#include <QtCore>
#ifdef Q_OS_WIN32
#include <stdlib.h>
#include <windows.h>
#include <sys/param.h>
#endif

static bool get_file_content(const QString &path, QByteArray &content, QString *error)
{
    QFile f(path);

    if (!f.open(QIODevice::ReadOnly))
    {
        if (error)
            *error = f.errorString();
        return false;
    }

    content = f.readAll();
    if (content.isEmpty() && f.error())
    {
        if (error)
            *error = f.errorString();
        return false;
    }

    return true;
}

static bool save_file(const QString &path, const QByteArray &data, QString *error)
{
    QTemporaryFile f(path + "XXXXXX");
    f.setAutoRemove(false);
    if (!f.open() || f.write(data) != data.size() || !f.flush())
    {
        if (error)
            *error = f.errorString();
        if (f.isOpen())
            f.remove();
        return false;
    }
    f.close();
    QFile tgt(path);
    if (tgt.exists() && !tgt.remove())
    {
        if (error)
            *error = tgt.errorString();
        f.remove();
        return false;
    }
    if (!f.rename(path))
    {
        if (error)
            *error = f.errorString();
        f.remove();
        return false;
    }
    return true;
}

QByteArray ggap::util::getFileContent(const QString &path, QString *error)
{
    QByteArray content;
    if (get_file_content(path, content, error))
        return content;
    else
        return QByteArray();
}

QString ggap::util::getFileText(const QString &path, QString *error)
{
    return QString::fromUtf8(getFileContent(path, error));
}

bool ggap::util::mkdir(const QString &path)
{
    // XXX permissions
    QDir d(path);
    return d.mkdir(".");
}

bool ggap::util::mkpath(const QString &path)
{
    // XXX permissions
    QDir d(path);
    return d.mkpath(".");
}

bool ggap::util::saveFile(const QString &path, const QString &content, QString *error)
{
    return save_file(path, content.toUtf8(), error);
}


#ifdef Q_OS_WIN32
static QString getWindir()
{
    wchar_t wdir[MAXPATHLEN];
    UINT n = GetWindowsDirectoryW(wdir, MAXPATHLEN);
    if (n > 0 && n < MAXPATHLEN)
        return QString::fromWCharArray(wdir, n);
    else
        return QString();
}

static QString getSystemDir()
{
    wchar_t wdir[MAXPATHLEN];
    UINT n = GetWindowsDirectoryW(wdir, MAXPATHLEN);
    if (n > 0 && n < MAXPATHLEN)
        return QString::fromWCharArray(wdir, n);
    else
        return QString();
}

static QStringList getExeExts()
{
    static QMutex m;
    QMutexLocker l(&m);

    static QStringList exts;

    if (exts.isEmpty())
    {
        QStringList comps;
        comps << ".exe" << ".cmd" << ".bat" << ".com";

        wchar_t *env = _wgetenv(L"PATHEXT");
        QString qenv = QString::fromWCharArray(env ? env : L"");
        comps << qenv.split(';');

        foreach (const QString &e, comps)
            if (!e.isEmpty() && !exts.contains(e, Qt::CaseInsensitive))
                exts << e;
    }

    return exts;
}
#endif

static QStringList getExeDirs()
{
    static QMutex m;
    QMutexLocker l(&m);

    static QStringList dirs;

    if (dirs.isEmpty())
    {
        QStringList comps;

#ifdef Q_OS_WIN32
        comps << getWindir() << getSystemDir() << ".";
        comps << QCoreApplication::applicationDirPath();
        wchar_t *env = _wgetenv(L"PATH");
        QString qenv = QString::fromWCharArray(env ? env : L"");
        comps << qenv.split(';');
#else
        QByteArray env = qgetenv("PATH");
        QString qenv = QString::fromLocal8Bit(env);
        comps = qenv.split(':');
#endif

        foreach (const QString &s, comps)
            if (!s.isEmpty())
                dirs << s;

#ifndef Q_OS_WIN32
        if (dirs.isEmpty())
            dirs << "/bin" << "/usr/bin";
#endif

        qDebug() << dirs;
    }

    return dirs;
}

static QString doFindProgramInPath(const QString &program)
{
    QFileInfo fi(program);

    if (fi.isAbsolute() || program.contains('/'))
    {
        if (fi.exists())
            return fi.absoluteFilePath();
        else
            return QString();
    }

    QStringList dirs = getExeDirs();

    foreach (const QString &d, dirs)
    {
        fi = d + "/" + program;
        if (fi.exists())
            return fi.absoluteFilePath();
    }

    return QString();
}

#ifdef Q_OS_WIN32
QString ggap::util::findProgramInPath(const QString &program)
{
    QStringList exts = getExeExts();

    foreach (const QString &e, exts)
        if (program.endsWith(e))
            return doFindProgramInPath(program);

    foreach (const QString &e, exts)
    {
        QString p = doFindProgramInPath(program + e);
        if (!p.isEmpty())
            return p;
    }

    return QString();
}
#else
QString ggap::util::findProgramInPath(const QString &program)
{
    return doFindProgramInPath(program);
}
#endif
