#include "ggap/script.h"
#include "ggap/worksheet.h"
#include <QtScript>
#include <QDesktopServices>

using namespace ggap;

WorksheetScriptProxy::WorksheetScriptProxy(Worksheet *ws) :
    QObject(ws), ws(ws)
{
}

WorksheetScriptProxy::~WorksheetScriptProxy()
{
}

bool WorksheetScriptProxy::openUrl(const QString &url)
{
    return QDesktopServices::openUrl(url);
}

static void bindMethods(QScriptValue &global_obj, const QScriptValue &self)
{
    QObject *obj = self.toQObject();
    const QMetaObject *meta = obj->metaObject();
    for (int i = 0; i < meta->methodCount(); ++i)
    {
        const QMetaMethod meth = meta->method(i);
        if (meth.access() == QMetaMethod::Public &&
            meth.methodType() == QMetaMethod::Method)
        {
            QString s = meth.signature();
            QString name = s.mid(0, s.indexOf('('));
            global_obj.setProperty(name, self.property(name));
        }
    }
}

void WorksheetScriptProxy::runScript(const QString &script)
{
    qDebug() << "running script:" << script;

    QScriptEngine engine;

    QScriptValue self = engine.newQObject(this);
    QScriptValue global_obj = engine.globalObject();
    global_obj.setProperty("worksheet", self);
    bindMethods(global_obj, self);

    qDebug() << engine.evaluate(script).toString();
}
