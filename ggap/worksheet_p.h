#ifndef GGAP_WORKSHEET_P_H
#define GGAP_WORKSHEET_P_H

#include <QStringListModel>
#include <QCompleter>
#include <QTextCursor>
#include "ggap/worksheet.h"

namespace ggap {

class Worksheet;

class WsCompleter : public QCompleter {
    Q_OBJECT

    Worksheet *ws;
    QStringListModel model;
    QStringList items;
    bool model_set;

private:
    QString getTextToComplete(QTextCursor &cr);
    void ensureModelSet();
    void unsetModel();
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void doActivated(const QString &text);
    void doUnsetModel();

public:
    WsCompleter(Worksheet *ws);
    void maybeComplete();
    void clear();

    void addData(const char *data, uint size);
    void removeData(const char *data, uint size);
};

namespace parser {
class ErrorInfo;
}

class WorksheetPrivate {
    M_DECLARE_PUBLIC(Worksheet)

    WorksheetScriptProxy *script_proxy;
    Worksheet::DocState doc_state;
    QString filename;
    mutable QString display_name;
    mutable QString display_basename;
    static int untitled_count;

    GapProcess *proc;
    bool allow_error_break;

    WsCompleter completer;

    File::Type file_type;

    int width;
    int height;
    bool resize_queued;

    void ensure_name() const
    {
        if (display_name.isEmpty())
        {
            if (untitled_count > 0)
                display_basename = QString("Untitled %1").arg(untitled_count + 1);
            else
                display_basename = QString("Untitled");
            untitled_count += 1;
            display_name = display_basename;
        }
    }

    bool isUntitled() const { return filename.isEmpty(); }
    QString displayName() const { ensure_name(); return display_name; }
    QString displayBasename() const { ensure_name(); return display_basename; }

    void startGap(const QString &workspace = QString());
    void forkCommand(const GapCommand &cmd);
    void killGap();
    void queueResize();
    void writeInput(const QStringList &lines, bool askCompletions = false);
    void writeFakeInput(const QStringList &lines, const QString &history, bool askCompletions = false);
    void writeErrors(const QList<parser::ErrorInfo> &parser);

    bool saveWorkspace(const QString &filename, QString &error);

    WorksheetPrivate(Worksheet *ws);
    ~WorksheetPrivate();
};

}

#endif // GGAP_WORKSHEET_P_H
