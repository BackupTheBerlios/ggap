#ifndef GGAP_SCRIPT_T_H
#define GGAP_SCRIPT_T_H

#include "ggap/script.h"
#include "ggap/worksheet.h"

namespace ggap {

class WorksheetScriptProxy::UnitTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void main()
    {
//         Worksheet ws;
//         WorksheetScriptProxy proxy(&ws);
//         proxy.runScript("openUrl('http://google.com')");
    }
};

}

#endif // GGAP_SCRIPT_T_H
