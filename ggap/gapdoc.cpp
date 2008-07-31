#include "ggap/gapdoc.h"
#include <QFile>
#include <QDebug>
#include <QStringList>

using namespace ggap;

namespace {

class SixFile {
    virtual bool parse(QFile *file) = 0;

protected:
    SixFile() {}

public:
    virtual ~SixFile() {}

    static SixFile *parseFile(const QString &filename);
};

struct Chapter {
    int no;
    QString file;
    QString title;

    Chapter(const QString &file, int no, const QString &title) :
        no(no), file(file), title(title)
    {
    }
};

struct Section {
    int mj, mn;
    QString title;

    Section(int mj, int mn, const QString &title) :
        mj(mj), mn(mn), title(title)
    {
    }
};

struct IndexEntry {
    int mj, mn;
    QString title;

    IndexEntry(int mj, int mn, const QString &title) :
        mj(mj), mn(mn), title(title)
    {
    }
};

struct Function {
    int mj, mn;
    QString title;

    Function(int mj, int mn, const QString &title) :
        mj(mj), mn(mn), title(title)
    {
    }
};

class SixFileDefault : public SixFile {
    QList<Chapter> chapters;
    QList<Section> sections;
    QList<IndexEntry> index_entries;
    QList<Function> functions;

    virtual bool parse(QFile *file);
};

}

SixFile *SixFile::parseFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
    {
        qCritical() << Q_FUNC_INFO << "could not open file"
                    << filename << "for reading: " << f.errorString();
        return 0;
    }

    SixFile *sf = 0;

    QByteArray line = f.readLine(1024);
    if (line.startsWith("#SIXFORMAT "))
    {
        QByteArray format = line.mid(strlen("#SIXFORMAT ")).simplified();
        qDebug() << Q_FUNC_INFO << format;

        if (format.toLower() != "default")
        {
            qCritical("%s: unknown format %s", Q_FUNC_INFO, format.data());
            return 0;
        }
    }
    else
    {
        f.seek(0);
    }

    if (!sf->parse(&f))
    {
        delete sf;
        sf = 0;
    }

    return sf;
}

bool SixFileDefault::parse(QFile *file)
{
    QStringList chaps, secs, idx, funcs;

    while (true)
    {
        QByteArray line = file->readLine(4096);

        if (line.isEmpty())
        {
            if (file->atEnd())
            {
                return true;
            }
            else
            {
                qWarning() << Q_FUNC_INFO << "error reading file" << file->fileName()
                           << ":" << file->errorString();
                return false;
            }
        }

        QString s = QString(line).simplified();
        if (s.isEmpty() || s.startsWith('#'))
            continue;

        if (s.length() < 3)
        {
            qWarning() << Q_FUNC_INFO << ": invalid manual.six:" << file->fileName();
            continue;
        }

        QString sub = s.mid(2);

        if (s.at(0) == 'C')
            chaps << sub;
        else if (s.at(0) == 'S')
            secs << sub;
        else if (s.at(0) == 'I')
            idx << sub;
        else if (s.at(0) == 'F')
        {
            if (funcs.isEmpty() || funcs.last() != sub)
                funcs << sub;
        }
        else
            qWarning() << Q_FUNC_INFO << ": invalid manual.six:" << file->fileName();;
    }

    QRegExp re_chaps("(\\S+)\\s+(\\d+)\\.\\s+\\S+");

    foreach (const QString &s, chaps)
    {
        if (!re_chaps.exactMatch(s))
            qWarning() << Q_FUNC_INFO << ": invalid six file line: " << s;
        else
            chapters << Chapter(re_chaps.cap(1), re_chaps.cap(2).toInt(), re_chaps.cap(3));
    }

    QRegExp re_secs("(\\d+)\\.(\\d+)\\.\\s*(\\S*)");

    foreach (const QString &s, secs)
    {
        if (!re_secs.exactMatch(s))
            qWarning() << Q_FUNC_INFO << ": invalid six file line: " << s;
        else
            sections << Section(re_secs.cap(1).toInt(), re_chaps.cap(2).toInt(), re_chaps.cap(3));
    }

    foreach (const QString &s, idx)
    {
        if (!re_secs.exactMatch(s))
            qWarning() << Q_FUNC_INFO << ": invalid six file line: " << s;
        else
            index_entries << IndexEntry(re_secs.cap(1).toInt(), re_chaps.cap(2).toInt(), re_chaps.cap(3));
    }

    foreach (const QString &s, funcs)
    {
        if (!re_secs.exactMatch(s))
            qWarning() << Q_FUNC_INFO << ": invalid six file line: " << s;
        else
            functions << Function(re_secs.cap(1).toInt(), re_chaps.cap(2).toInt(), re_chaps.cap(3));
    }

    return true;
}
