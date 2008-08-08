#ifndef GGAP_GAP_P_H
#define GGAP_GAP_P_H

#include "ggap/app.h"
#include "ggap/gap.h"
#include <QStringList>
#include <QString>
#include <QFileInfo>
#include <QSettings>
#include <QDateTime>
#include <QMap>
#include <QSet>

namespace ggap {

struct GapConfig {
    QString exe;
    QString rootDir;
    QStringList extraRootDirs;
    QString args;

    static GapConfig current()
    {
        GapConfig cfg;
        cfg.exe = GapOptions().exe();
        cfg.rootDir = GapOptions().rootDir();
        cfg.args = GapOptions().args();
        cfg.extraRootDirs = prefsValue(Prefs::ExtraGapRoots);
        return cfg;
    }

    bool isEmpty() const
    {
        return exe.isEmpty();
    }

    bool operator == (const GapConfig &other) const
    {
        return exe == other.exe &&
               rootDir == other.rootDir &&
               extraRootDirs == other.extraRootDirs &&
               args == other.args;
    }

    bool operator != (const GapConfig &other) const
    {
        return !(*this == other);
    }
};

struct GapConfigStampSet {
    QStringList files;
    QDateTime stamp;
};

class GapConfigCache {
    GapConfig lastCfg;

    struct FileStampSet {
        QMap<QString, QDateTime> data;

        void addFiles(const QStringList &files)
        {
            foreach (const QString &f, files)
            {
                QFileInfo fi(f);
                if (fi.exists())
                    data[f] = fi.lastModified();
            }
        }

        explicit FileStampSet(const QStringList &files)
        {
            addFiles(files);
        }
    };

    FileStampSet watchedFiles(const GapConfig &cfg)
    {
        QStringList list;

        list << cfg.exe;
        list << cfg.rootDir + "/lib/init.g";

        QStringList root_dirs;
        root_dirs << cfg.rootDir << cfg.extraRootDirs;

        foreach (const QString &dir, root_dirs)
        {
            QFileInfoList pkg_dirs = QDir(dir + "/pkg").entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
            foreach (const QFileInfo &fi, pkg_dirs)
                list << fi.filePath() + "/init.g"
                     << fi.filePath() + "/read.g"
                     << fi.filePath() + "/PackageInfo.g";
        }

        return FileStampSet(list);
    }

    static bool makeWorkspaceFileName(const FileStampSet &stamps,
                                      QString            *wsp_file,
                                      const QString      &ini_file)
    {
        *wsp_file = gapApp->getUserDataFile("workspace");

        QSettings ini(ini_file, QSettings::IniFormat);
        ini.setValue("Workspace/version", "1.0");
        ini.setValue("Workspace/workspace", *wsp_file);
        ini.setValue("Workspace/files", QStringList(stamps.data.keys()));
        ini.sync();

        return false;
    }

public:
    bool checkSavedWorkspace(const GapConfig &cfg, QString *wsp_file_p)
    {
        if (lastCfg.isEmpty())
            lastCfg = cfg;

        QString ini_file = gapApp->getUserDataFile("workspace.ini");

        FileStampSet new_stamps = watchedFiles(cfg);

        if (lastCfg != cfg)
        {
            lastCfg = cfg;
            return makeWorkspaceFileName(new_stamps, wsp_file_p, ini_file);
        }

        if (!QFileInfo(ini_file).exists())
        {
            qDebug("ini file '%s' does not exist", qPrintable(ini_file));
            return makeWorkspaceFileName(new_stamps, wsp_file_p, ini_file);
        }

        QSettings ini(ini_file, QSettings::IniFormat);
        if (ini.value("Workspace/version") != "1.0")
        {
            qDebug("wrong version in ini file '%s'", qPrintable(ini_file));
            return makeWorkspaceFileName(new_stamps, wsp_file_p, ini_file);
        }

        QString wsp_file = ini.value("Workspace/workspace").toString();
        QFileInfo wsp_info(wsp_file);
        if (!wsp_info.exists())
            wsp_info.setFile(wsp_file + ".gz");
        if (!wsp_info.exists())
        {
            qDebug("workspace file %s does not exist", qPrintable(wsp_file));
            return makeWorkspaceFileName(new_stamps, wsp_file_p, ini_file);
        }

        QDateTime wsp_stamp = wsp_info.lastModified();

        QStringList old_files = ini.value("Workspace/files").toStringList();
        QSet<QString> new_filenames, old_filenames;
        for (QMap<QString, QDateTime>::const_iterator iter = new_stamps.data.begin();
             iter != new_stamps.data.end(); ++iter)
                new_filenames << iter.key();
        foreach (const QString &p, old_files)
            old_filenames << p;

        if (new_filenames != old_filenames)
        {
            qDebug() << "old:" << old_filenames;
            qDebug() << "new:" << new_filenames;
            return makeWorkspaceFileName(new_stamps, wsp_file_p, ini_file);
        }

        for (QMap<QString, QDateTime>::const_iterator iter = new_stamps.data.begin();
             iter != new_stamps.data.end(); ++iter)
        {
            if (wsp_stamp < iter.value())
            {
                qDebug() << iter.key() << "is newer than" << wsp_file;
                return makeWorkspaceFileName(new_stamps, wsp_file_p, ini_file);
            }
        }

        *wsp_file_p = wsp_file;
        return true;
    }
};

} // namespace ggap

#endif // GGAP_GAP_P_H
