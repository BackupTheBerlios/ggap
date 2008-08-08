#ifndef GGAP_GAP_P_H
#define GGAP_GAP_P_H

#include <QStringList>
#include <QString>
#include <QFileInfo>
#include <QSettings>
#include <QDateTime>

namespace ggap {

struct GapConfig {
    QString exe;
    QString rootDir;
    QStringList extraRootDirs;
    QString args;

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

    QFileInfoList watchedFiles(const GapConfig &cfg)
    {
        QFileInfoList list;

        list << QFileInfo(cfg.exe);
        list << QFileInfo(cfg.rootDir + "/lib/init.g");

        QStringList root_dirs;
        root_dirs << cfg.rootDir << cfg.extraRootDirs;

        foreach (const QString &dir, root_dirs)
        {
            QStringList pkg_dirs = QDir(dir + "/pkg").entryList(QDir::Dirs);
            foreach (const QString &pd, pkg_dirs)
            {
                QFileInfo fi(pd + "/init.g");
                if (fi.exists())
                    list << fi;
                fi.setFile(pd + "/read.g");
                if (fi.exists())
                    list << fi;
            }
        }

        return list;
    }

    bool checkSavedWorkspace(const GapConfig &cfg, const QString &ini_file, QString *wsp_file_p)
    {
        if (lastCfg.isEmpty())
            lastCfg = cfg;

        if (lastCfg != cfg)
        {
            lastCfg = cfg;
            return false;
        }

        if (!QFileInfo(ini_file).exists())
        {
            qDebug("ini file '%s' does not exist", qPrintable(ini_file));
            return false;
        }

        QSettings ini(ini_file);
        if (ini.value("Workspace/version") != "1.0")
        {
            qDebug("wrong version in ini file '%s'", qPrintable(ini_file));
            return false;
        }

        QString wsp_file = ini.value("Workspace/workspace").toString();
        QFileInfo wsp_info(wsp_file);

        if (!wsp_info.exists())
        {
            qDebug("workspace file %s does not exist", qPrintable(wsp_file));
            return false;
        }

        QDateTime wsp_stamp = wsp_info.lastModified();

        QStringList old_files = ini.value("Workspace/files").toStringList();
        QFileInfoList new_files = watchedFiles(cfg);
        QSet<QString> new_filenames, old_filenames;
        foreach (const QFileInfo &fi, new_files)
            new_filenames << fi.filePath();
        foreach (const QString &p, old_files)
            old_filenames << p;

        qDebug() << "old:" << old_filenames;
        qDebug() << "new:" << new_filenames;

        if (new_filenames != old_filenames)
            return false;

        foreach (const QFileInfo &fi, new_files)
        {
            if (wsp_stamp < fi.lastModified())
            {
                qDebug() << fi.filePath() << "is newer than" << wsp_file;
                return false;
            }
        }

        *wsp_file_p = wsp_file;
        return true;
    }
};

} // namespace ggap

#endif // GGAP_GAP_P_H
