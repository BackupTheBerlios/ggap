#ifndef GGAP_SCRIPT_H
#define GGAP_SCRIPT_H

#include <QObject>

namespace ggap {

class Worksheet;

class WorksheetScriptProxy : public QObject {
    Q_OBJECT

    Worksheet *ws;

public:
    WorksheetScriptProxy(Worksheet *ws);
    ~WorksheetScriptProxy();

    void runScript(const QString &script);

    Q_INVOKABLE bool openUrl(const QString &url);
};

}

#endif // GGAP_SCRIPT_H
