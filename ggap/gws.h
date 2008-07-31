#ifndef GGAP_GWS_H
#define GGAP_GWS_H

#include <QFile>
#include <moo-unit-tests.h>

namespace ggap {

class GwsReaderPrivate;
class GwsReader {
    friend class GwsReaderPrivate;
    GwsReaderPrivate *impl;
    MOO_DECLARE_UNIT_TESTS()

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
    MOO_DECLARE_UNIT_TESTS()

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
