#include "config.h"
#include "ggap/gap-p.h"
#include "ggap/app.h"
#include "ggap/utils.h"
#include "moo-macros.h"
#include <QtCore>
#ifdef Q_OS_WIN32
#include <windows.h>
#endif

using namespace ggap;

#ifdef Q_OS_WIN32
static QString filenameToCommandLine(const QString &path)
{
    QString path_sl = QDir::toNativeSeparators(path);
    int len = path_sl.length();
    QVector<wchar_t> path_w(len + 1);
    path_sl.toWCharArray(path_w.data());
    path_w[len] = 0;

    QVector<wchar_t> dest(len + 1);
    int ret = GetShortPathNameW(path_w.constData(), dest.data(), dest.size());
    if (ret > dest.size())
    {
        dest.resize(ret);
        ret = GetShortPathNameW(path_w.constData(), dest.data(), dest.size());
    }

    if (ret <= 0)
    {
        qCritical("%s: oops", Q_FUNC_INFO);
        return QDir::fromNativeSeparators(path);
    }
    else
    {
        return QDir::fromNativeSeparators(QString::fromWCharArray(dest.constData()));
    }
}
#else
static QString filenameToCommandLine(const QString &path)
{
    return path;
}
#endif

static QString getGapRootFromBinDir(const QDir &bindir)
{
    if (!bindir.exists())
        return QString();
    if (bindir.dirName() != "bin")
        return QString();
    QDir root = bindir;
    root.cdUp();
    if (!root.exists("grp") || !root.exists("pkg") || !root.exists("lib"))
        return QString();
    else
        return root.path();
}

QString GapOptions::defaultRootDir()
{
    QString val = QCoreApplication::applicationDirPath() + "/gap4r4";
    if (QFileInfo(val).isDir())
        return val;
    else
        return QString();
}

QString GapOptions::rootDir()
{
    if (prefsValue(Prefs::UseDefaultGap))
    {
        QString val = defaultRootDir();
        if (!val.isEmpty())
            return val;
    }

    QString val = prefsValue(Prefs::GapRootDir);

    if (!val.isEmpty())
        return val;

    QString exe = this->exe();

    if (exe.isEmpty())
        return QString();

    QFileInfo fi(exe);
    m_return_val_if_fail(fi.isAbsolute(), QString());

    {
        QString tmp = fi.symLinkTarget();
        if (!tmp.isEmpty())
        {
            exe = tmp;
            fi = QFileInfo(exe);
            if (!fi.isAbsolute())
                return QString();
        }
    }

    QDir dir = fi.dir();
    return getGapRootFromBinDir(dir);
}

QString GapOptions::docDir()
{
    QString root = rootDir();
    if (!root.isEmpty() && QDir(root + "/doc").exists())
        return root + "/doc";
    else
        return QString();
}

QString GapOptions::defaultExe()
{
    QString val;

#ifdef Q_OS_WIN32
    val = QCoreApplication::applicationDirPath() + "/gap4r4/bin/gapw95.exe";
#else
    val = QCoreApplication::applicationDirPath() + "/gap4r4/bin/gap.sh";
#endif

    if (QFileInfo(val).exists())
        return val;
    else
        return QString();
}

QString GapOptions::exe()
{
    QString val;

    if (prefsValue(Prefs::UseDefaultGap))
    {
        val = defaultExe();
        if (!val.isEmpty())
            return val;
    }

    val = prefsValue(Prefs::GapExe);

    if (val.isEmpty())
    {
#ifdef Q_OS_WIN32
        val = "c:/gap4r4/bin/gapw95.exe";
#else
        val = "gap";
#endif

        val = util::findProgramInPath(val);
    }

    return val;
}

QString GapOptions::args()
{
    return prefsValue(Prefs::GapArgs);
}

static QString escape_text(const QString &text)
{
    QString fn = text;
    fn.replace("\\", "\\\\").replace("\b", "\\b").replace("\f", "\\f")
      .replace("\t", "\\t").replace("\r", "\\r").replace("\n", "\\n")
      .replace("\"", "\\\"");
    return fn;
}

static QString filenameToGapString(const QString &path)
{
    return escape_text(filenameToCommandLine(path));
}

#define SAVE_WORKSPACE \
"SaveWorkspace(\"%1\");\n"

QString ggap::gapCmdRunCommand(const QString &cmdname, const QString &args)
{
    if (!args.isEmpty())
        return QString("$GGAP_EXEC_COMMAND(\"%1\", %2);\n").arg(cmdname).arg(args);
    else
        return QString("$GGAP_EXEC_COMMAND(\"%1\");\n").arg(cmdname);
}

QString ggap::gapCmdSaveWorkspace(const QString &filename)
{
    return QString(SAVE_WORKSPACE).arg(filenameToGapString(filename));
}

QString ggap::gapCmdHelp(const QString &text)
{
    return QString("$GGAP_HELP(\"%1\");").arg(escape_text(text));
}


// DO NOT replace rm -f with RemoveFile():
// "rm -f" is broken when the filename contains spaces: GAP doesn't escape anything;
// but fixing it will break GAP which won't be able to un'gzip the workspace file.
#define SAVE_WORKSPACE_AND_GZIP                         \
SAVE_WORKSPACE                                          \
"if ARCH_IS_UNIX() then\n"                              \
"  Exec(\"rm -f\", Concatenation(\"%2\", \".gz\"));\n"  \
"  Exec(\"gzip\", \"%3\");\n"                           \
"fi;\n"

static QString save_workspace_init_file(const QString &workspace)
{
    m_return_val_if_fail(!workspace.isEmpty(), QString());

    QString wsp_escaped = filenameToGapString(workspace);
    QString contents = QString(SAVE_WORKSPACE_AND_GZIP)
                        .arg(wsp_escaped, wsp_escaped, wsp_escaped);

    QString error;
    QString filename = gapApp->saveTempFile(contents, error);

    if (filename.isEmpty())
        qCritical() << Q_FUNC_INFO << ": " << error;

    return filename;
}


// static char *
// gap_file_func_string (const char *filename,
//                       const char *func)
// {
//     char *escaped, *string;
//
//     g_return_val_if_fail (filename != NULL, NULL);
//     g_return_val_if_fail (func != NULL, NULL);
//
//     escaped = gap_escape_filename (filename);
//     string = g_strdup_printf ("%s(\"%s\");\n", func, escaped);
//
//     g_free (escaped);
//     return string;
// }
//
//
// char*
// gap_read_file_string (const char *filename)
// {
//     return gap_file_func_string (filename, "Read");
// }
//
//
// char*
// gap_reread_file_string (const char *filename)
// {
//     return gap_file_func_string (filename, "Reread");
// }


static QString get_restore_g()
{
    static QString restore_g;

    if (restore_g.isEmpty())
    {
        QString ggap_g = gapApp->fileNameInTempDir("ggap.g");

        QFile source(":/gap/ggap.g");
        if (!source.copy(ggap_g))
        {
            qCritical() << QString("%1: could not create file %2: %3")
                                    .arg(Q_FUNC_INFO).arg(ggap_g)
                                    .arg(source.errorString());
            ggap_g = QString();
        }
        QFile::setPermissions(ggap_g, QFile::ReadOwner | QFile::WriteOwner);
        qDebug() << "created" << ggap_g;

        QString text = util::getFileText(":/gap/restore.g");
        text = text.arg(GGAP_API_VERSION, ggap_g, ggap_g);

        QString error;
        restore_g = gapApp->fileNameInTempDir("restore.g");
        if (!util::saveFile(restore_g, text, &error))
        {
            qCritical() << QString("%1: could not create file %2: %3")
                                    .arg(Q_FUNC_INFO).arg(restore_g)
                                    .arg(error);
            restore_g = QString();
        }
        qDebug() << "created" << restore_g;
    }

    return restore_g;
}

static GapCommand make_command_line(const QString     &cmd_base,
                                    const QStringList &flags,
                                    const QString     &custom_wsp)
{
    QStringList prefix;
    prefix << cmd_base;

#ifdef Q_OS_WIN32
    if (cmd_base.endsWith("gapw95.exe", Qt::CaseInsensitive))
    {
        QString root = GapOptions().rootDir();
        if (root.isEmpty())
            qCritical("%s: oops", Q_FUNC_INFO);
        else
            prefix << "-l" << filenameToCommandLine(root);
    }
#endif

    QStringList extra_roots = prefsValue(Prefs::ExtraGapRoots);
    foreach (const QString &d, extra_roots)
        prefix << "-l" << d + ";";

    QStringList wsp1, wsp2;
    bool use_2 = false;
    bool save_workspace = prefsValue(Prefs::SaveWorkspace);
    bool wsp_already_saved = false;
    GapCommand::Flags cmd_flags;

    QString wsp_file;
    static GapConfigCache cache;

    if (custom_wsp.isEmpty() && save_workspace)
        wsp_already_saved = cache.checkSavedWorkspace(GapConfig::current(), &wsp_file);

    if (!custom_wsp.isEmpty())
    {
        wsp1 << "-L" << filenameToCommandLine(custom_wsp);
        use_2 = true;
        cmd_flags = GapCommand::UserWorkspace;
    }
    else if (save_workspace)
    {
        if (!wsp_already_saved && !gapApp->makeUserDataDir())
            qCritical("%s: could not create user data dir", Q_FUNC_INFO);

        if (wsp_already_saved)
        {
            wsp1 << "-L" << filenameToCommandLine(wsp_file);
            wsp2 << filenameToCommandLine(save_workspace_init_file(wsp_file));
            use_2 = true;
        }
        else
        {
            wsp1 << filenameToCommandLine(save_workspace_init_file(wsp_file));
        }
    }

    QStringList suffix;
    QString restore_g = get_restore_g();
    if (restore_g.isEmpty())
        qCritical("%s: could not generate restore.g file!", Q_FUNC_INFO);
    else
        suffix << filenameToCommandLine(restore_g);

    QStringList cmd_line1, cmd_line2;
    cmd_line1 << prefix << wsp1 << suffix;
    if (use_2)
        cmd_line2 << prefix << wsp2 << suffix;

    qDebug() << cmd_line1;
    qDebug() << cmd_line2;
    return GapCommand(cmd_flags, cmd_line1, cmd_line2);
}

// Function from Qt's qprocess.cpp
static QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}

GapCommand ggap::makeGapCommand(const QString &workspace, const QStringList &flags)
{
    GapOptions go;
    QString cmd_base = go.exe();
    if (cmd_base.isEmpty())
    {
        qCritical("%s: gap command line not set", Q_FUNC_INFO);
        cmd_base = "gap";
    }

    QStringList all_flags = parseCombinedArgString(go.args());
    all_flags << flags;
    return make_command_line(cmd_base, all_flags, workspace);
}
