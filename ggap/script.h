#ifndef GGAP_SCRIPT_H
#define GGAP_SCRIPT_H

#include <QObject>
#include <moo-unit-tests.h>

namespace ggap {

class Worksheet;

class WorksheetScriptProxy : public QObject {
    Q_OBJECT
    MOO_DECLARE_UNIT_TESTS()

    Worksheet *ws;

public:
    WorksheetScriptProxy(Worksheet *ws);
    ~WorksheetScriptProxy();

    void runScript(const QString &script);

    Q_INVOKABLE bool openUrl(const QString &url);
};

}

#endif // GGAP_SCRIPT_H
