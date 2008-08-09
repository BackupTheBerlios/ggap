#include "config.h"
#include "ggap/gws.h"
#include "ggap/app.h"
#include "zip/zip.h"
#include "zip/unzip.h"
#include <QtCore>
#include "moo-macros.h"

using namespace ggap;

/**************************************************************************
 *
 * gws file is a zipped folder containing the following files:
 *  info.ini - meta info about the file
 *  workspace - saved workspace
 *  worksheet.xml - worksheet
 *
 */

namespace ggap {

struct GwsInfo {
    QString workspace;
    QString worksheet;
    bool load(const QString &path, QString &error);
    void save(const QString &path);
};

#define INFO_INI "info.ini"
#define KEY_GWS_VERSION "gws/version"
#define CURRENT_VERSION "1.0"
#define KEY_GWS_WORKSHEET "gws/worksheet"
#define KEY_GWS_WORKSPACE "gws/workspace"
#define KEY_GWS_DATE_SAVED "gws/date-saved"
#define KEY_GWS_GGAP_VERSION "gws/ggap-version"

bool GwsInfo::load(const QString &path, QString &error)
{
    QSettings s(path, QSettings::IniFormat);

    QString version = s.value(KEY_GWS_VERSION).toString();
    if (version != CURRENT_VERSION)
    {
        error = QString("invalid version '%1'").arg(version);
        return false;
    }

    workspace = s.value(KEY_GWS_WORKSPACE).toString();
    worksheet = s.value(KEY_GWS_WORKSHEET).toString();
    return true;
}

void GwsInfo::save(const QString &path)
{
    QSettings s(path, QSettings::IniFormat);
    s.setValue(KEY_GWS_VERSION, CURRENT_VERSION);
    s.setValue(KEY_GWS_WORKSHEET, worksheet);

    s.setValue(KEY_GWS_DATE_SAVED, QDateTime::currentDateTime().toString(Qt::ISODate));
    s.setValue(KEY_GWS_GGAP_VERSION, GGAP_VERSION);

    if (!workspace.isEmpty())
        s.setValue(KEY_GWS_WORKSPACE, workspace);
    s.sync();
}

} // namespace ggap

static void deleteDir(QDir &dir)
{
    QStringList files = dir.entryList();
    foreach (const QString &name, files)
        QFile::remove(dir.filePath(name));

    QFileInfo fi(dir.path());
    QDir(fi.path()).rmdir(fi.fileName());
}

/**************************************************************************
 *
 * GwsReader
 *
 */

struct ggap::GwsReaderPrivate {
    QString file;
    QString dir;
    QString workspace;
    QString worksheet;
    GwsReader *reader;

    GwsReaderPrivate(GwsReader *reader, const QString &file);
    ~GwsReaderPrivate();
    bool unpack(QString &error);
};

GwsReaderPrivate::GwsReaderPrivate(GwsReader *reader, const QString &file) :
    file(file), reader(reader)
{
}

GwsReaderPrivate::~GwsReaderPrivate()
{
}

bool GwsReaderPrivate::unpack(QString &error)
{
    m_return_val_if_fail(dir.isEmpty(), false);

    UnZip uz;
    UnZip::ErrorCode ec = uz.openArchive(file);
    if (ec != UnZip::Ok)
    {
        error = QString("openArchive failed: ") + uz.formatError(ec);
        return false;
    }

    if (!uz.contains(INFO_INI))
    {
        error = "Invalid file";
        return false;
    }

    QDir d = gapApp->makeTempDir();
    if ((ec = uz.extractFile(INFO_INI, d)))
    {
        deleteDir(d);
        error = QString("extractFile failed: ") + uz.formatError(ec);
        return false;
    }

    GwsInfo info;
    if (!info.load(d.filePath(INFO_INI), error))
    {
        deleteDir(d);
        return false;
    }

    if ((ec = uz.extractFile(info.worksheet, d)) ||
        (!info.workspace.isEmpty() && (ec = uz.extractFile(info.workspace, d))))
    {
        deleteDir(d);
        error = QString("extractFile failed for worksheet or workspace: ") + uz.formatError(ec);
        return false;
    }

    if (!info.workspace.isEmpty())
        workspace = d.filePath(info.workspace);
    if (!info.worksheet.isEmpty())
        worksheet = d.filePath(info.worksheet);
    dir = d.absolutePath();
    return true;
}

GwsReader::GwsReader(const QString &file) :
    impl(new GwsReaderPrivate(this, file))
{
}

GwsReader::~GwsReader()
{
    delete impl;
}

bool GwsReader::unpack(QString &error)
{
    return impl->unpack(error);
}

QString GwsReader::workspace()
{
    return impl->workspace;
}

QString GwsReader::worksheet()
{
    return impl->worksheet;
}


/**************************************************************************
 *
 * GwsWriter
 *
 */

struct ggap::GwsWriterPrivate {
    QString dest;
    QDir dir;
    GwsInfo info;
    bool closed;

    GwsWriterPrivate(const QString &dest) :
        dest(dest), closed(true)
    {
    }
};

GwsWriter::GwsWriter(const QString &dest) :
    impl(new GwsWriterPrivate(dest))
{
}

GwsWriter::~GwsWriter()
{
    close();
    delete impl;
}

bool GwsWriter::open(QString &)
{
    m_return_val_if_fail(impl->closed, false);
    impl->closed = false;
    impl->dir = gapApp->makeTempDir();
    return true;
}

QString GwsWriter::workspaceFilename()
{
    m_return_val_if_fail(!impl->closed, QString());
    QString workspace = "workspace";
    impl->info.workspace = workspace;
    return impl->dir.filePath(workspace);
}

bool GwsWriter::addWorksheet(const QByteArray &xml, QString &error)
{
    m_return_val_if_fail(!impl->closed, false);
    QString worksheet = "worksheet.xml";
    QString path = impl->dir.filePath(worksheet);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        error = file.errorString();
        return false;
    }
    if (file.write(xml) != xml.size() || !file.flush())
    {
        error = file.errorString();
        return false;
    }
    file.close();
    impl->info.worksheet = worksheet;
    return true;
}

bool GwsWriter::write(QString &error)
{
    m_return_val_if_fail(!impl->closed, false);
    impl->info.save(impl->dir.filePath(INFO_INI));

    QTemporaryFile *tmp_file = new QTemporaryFile(impl->dest + "XXXXXX");
    tmp_file->setAutoRemove(false);

    if (!tmp_file->open())
    {
        error = tmp_file->errorString();
        delete tmp_file;
        return false;
    }

    QString tmp_path = tmp_file->fileName();

    Zip zip;
    Zip::ErrorCode ec;
    if ((ec = zip.createArchive(tmp_file)) ||
        (ec = zip.addDirectoryContents(impl->dir.path(), Zip::Deflate3)) ||
        (ec = zip.closeArchive()))
    {
        error = zip.formatError(ec);
        return false;
    }

    QFile d(impl->dest);
    QFile f(tmp_path);
    if (d.exists() && !d.remove())
    {
        error = d.errorString();
        f.remove();
        return false;
    }
    if (!f.rename(impl->dest))
    {
        error = f.errorString();
        f.remove();
        return false;
    }

    return true;
}

void GwsWriter::close()
{
    if (impl->closed)
        return;

    deleteDir(impl->dir);
    impl->closed = true;
}
