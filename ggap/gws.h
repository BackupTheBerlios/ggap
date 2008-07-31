#ifndef GGAP_GWS_H
#define GGAP_GWS_H

#include <QFile>

namespace ggap {

class GwsReaderPrivate;
class GwsReader {
    friend class GwsReaderPrivate;
    GwsReaderPrivate *impl;
public:
    GwsReader(const QString &file);
    ~GwsReader();

    bool unpack(QString &error);
    QString workspace();
    QString worksheet();
};

class GwsWriterPrivate;
class GwsWriter {
    friend class GwsWriterPrivate;
    GwsWriterPrivate *impl;

public:
    GwsWriter(const QString &dest);
    ~GwsWriter();

    bool open(QString &error);
    bool write(QString &error);
    void close();

    QString workspaceFilename();
    bool addWorksheet(const QByteArray &xml, QString &error);
};

}

#endif // GGAP_GWS_H
