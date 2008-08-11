#include "ggap/worksheet-p.h"
#include "ggap/proc.h"
#include "ggap/gap.h"
#include "ggap/app.h"
#include "ggap/parser.h"
#include "ggap/gws.h"
#include "ggap/script.h"
#include "ggap/help.h"
#include "moows/wstextblock.h"
#include "moows/wstextdocument.h"
#include "moo-macros.h"
#include <QtGui>
#include <string.h>

using namespace ggap;

int WorksheetPrivate::untitled_count;

WorksheetPrivate::WorksheetPrivate(Worksheet *ws) :
    pub(ws),
    script_proxy(new WorksheetScriptProxy(ws)),
    doc_state(Worksheet::Idle),
    filename(), display_name(), display_basename(),
    proc(0), allow_error_break(false),
    completer(ws),
    file_type(File::Workspace),
    width(-1), height(-1), resize_queued(false)
{
}

WorksheetPrivate::~WorksheetPrivate()
{
    M_ASSERT(!proc);
}

void Worksheet::setFilename(const QString &filename)
{
    if (filename == impl->filename)
        return;

    if (filename.isEmpty())
    {
        impl->filename = QString();
        impl->display_name = QString();
        impl->display_basename = QString();
    }
    else
    {
        QFileInfo fi(filename);
        impl->filename = fi.absoluteFilePath(); // XXX
        impl->display_name = QDir::toNativeSeparators(impl->filename);
        impl->display_basename = fi.fileName();
    }

    emit filenameChanged(impl->filename);
}

QString Worksheet::filename() const
{
    return impl->filename;
}

QString Worksheet::displayName() const
{
    return impl->displayName();
}

QString Worksheet::displayBasename() const
{
    return impl->displayBasename();
}

bool Worksheet::isModified() const
{
    return document().isModified();
}

void Worksheet::setModified(bool modified)
{
    document().setModified(modified);
}

bool Worksheet::isUntitled() const
{
    return impl->isUntitled();
}

void Worksheet::setDocState(DocState state)
{
    if (impl->doc_state != state)
    {
        impl->doc_state = state;
        emit docStateChanged();
    }
}

Worksheet::DocState Worksheet::docState() const
{
    return impl->doc_state;
}

Worksheet::Worksheet(QWidget *parent) :
    moo::ws::Worksheet(parent),
    impl(this)
{
    applyPrefs();
    connect(&document(), SIGNAL(modificationChanged(bool)),
            SIGNAL(modificationChanged(bool)));
    document().setHrefParser(HelpBrowser::parseUrl);
}

Worksheet::~Worksheet()
{
    impl->killGap();
}


void Worksheet::applyPrefs()
{
    setFont(prefsValue(Prefs::WorksheetFont));
}


void Worksheet::start()
{
    m_return_if_fail(!impl->proc);
    startInput("gap> ");
    impl->startGap();
    setAcceptingInput(false);
    setModified(false);
}

bool Worksheet::isRunning()
{
    return impl->proc;
}

void WorksheetPrivate::startGap(const QString &workspace)
{
    QStringList args;
    args << "-n";
    if (!allow_error_break)
        args << "-T";

    completer.clear();

    GapCommand cmd = makeGapCommand(workspace, args);
//     moo_worksheet_reset (MOO_WORKSHEET (view));
    forkCommand(cmd);
}

GapProcess::State Worksheet::gapState() const
{
    return impl->proc ? impl->proc->state() : GapProcess::Dead;
}

void Worksheet::resize_idle()
{
    impl->resize_queued = false;

    QSize sz = size();

    if (sz.isNull())
    {
        impl->width = -1;
        impl->height = -1;
        return;
    }

//     layout = gtk_widget_create_pango_layout (GTK_WIDGET (ws), " ");
//     pango_layout_get_pixel_size (layout, &width, &height);
//
//     text_width = allocation->width -
//                     gtk_text_view_get_left_margin (GTK_TEXT_VIEW (ws)) -
//                     gtk_text_view_get_right_margin (GTK_TEXT_VIEW (ws));
//
// #define HOW_MANY(x__,y__) (((x__) + (y__) - 1) / (y__))
//     ws->priv->width = HOW_MANY (text_width, width) - MOO_WORKSHEET_OUTPUT_INDENT;
//     ws->priv->height = HOW_MANY (allocation->height, height);
// #undef HOW_MANY
//
//     /* leave one char to make sure horizontal scrollbar won't appear */
//     ws->priv->width = MAX (ws->priv->width - 1, 10);
//     ws->priv->height = MAX (ws->priv->height, 10);
//     g_object_unref (layout);

    if (impl->proc)
        impl->proc->setWindowSize(impl->width, impl->height);
}

void WorksheetPrivate::queueResize()
{
    if (!resize_queued)
    {
        resize_queued = true;
        M_Q(Worksheet);
        QTimer::singleShot(0, q, SLOT(resize_idle()));
    }
}

void Worksheet::resizeEvent(QResizeEvent *e)
{
    moo::ws::Worksheet::resizeEvent(e);
    impl->queueResize();
}


bool Worksheet::isEmpty() const
{
    if (isModified() || !isUntitled())
        return false;

    const moo::ws::PromptBlock *block;
    block = dynamic_cast<const moo::ws::PromptBlock*>(firstBlock());
    return block && !block->next() && block->text().isEmpty();
}


void WorksheetPrivate::writeInput(const QStringList &lines, bool askCompletions)
{
    m_return_if_fail(proc);
    proc->writeInput(lines, askCompletions);
    M_Q(Worksheet);
    q->addHistory(lines.join("\n"));
}

void WorksheetPrivate::writeFakeInput(const QStringList &lines, const QString &history, bool askCompletions)
{
    m_return_if_fail(proc);
    proc->writeInput(lines, askCompletions);
    M_Q(Worksheet);
    q->addHistory(history);
}

void WorksheetPrivate::writeErrors(const QList<parser::ErrorInfo> &list)
{
    int line = -1, column = -1;
    M_Q(Worksheet);

    if (list.isEmpty())
    {
        line = 0;
        column = 0;
        q->writeError("Syntax error");
    }
    else foreach (const parser::ErrorInfo &e, list)
    {
        q->highlightError(e.line, e.firstColumn, e.lastColumn, e.message);
        if (line < 0)
        {
            line = e.line;
            column = e.firstColumn;
        }
    }

    q->resumeInput(line, column);
}

void Worksheet::processInput(const QStringList &lines)
{
    m_return_if_fail(impl->proc && impl->proc->state() == GapProcess::InPrompt);

    resetHistory();

    QString firstLine;

    if (lines.size() == 1 && (firstLine = lines.at(0).simplified()).startsWith("?"))
    {
        // Help
        QStringList list;
        list << gapCmdHelp(firstLine.mid(1).simplified());
        impl->writeFakeInput(list, lines.at(0), false);
    }
    else
    {
        parser::Text result;
        QList<parser::ErrorInfo> errors;

        switch (parser::parse(lines, &result, &errors))
        {
            case parser::Success:
                // FIXME store whole thing in the input block
                impl->writeInput(result.lines, true);
                break;

            case parser::Incomplete:
                continueInput();
                break;

            case parser::Error:
                impl->writeErrors(errors);
                break;
        }
    }
}


void Worksheet::gapStarted(GapProcess::StartResult result, const QString &output)
{
    qDebug() << Q_FUNC_INFO << result << output;
    // XXX XXX show dialog
}

void Worksheet::gapError(int exitCode, QProcess::ExitStatus exitStatus, const QString &output)
{
    qDebug() << Q_FUNC_INFO << output;

#warning "Worksheet::gapError: Fix me already"

    QString msg;

//     switch (error)
//     {
//         case QProcess::FailedToStart:
//             msg = "GAP failed to start";
//             break;
//         case QProcess::Crashed:
//             msg = "GAP crashed";
//             break;
//         default:
//             msg = "An error occurred";
//             break;
//     }

    bool was_modified = isModified();
    if (isEmpty())
        reset();

    // XXX XXX show dialog

    writeError(msg);
    setModified(was_modified);
    emit gapStateChanged();
}

void Worksheet::gapDied(int exitCode, QProcess::ExitStatus exitStatus)
{
    // XXX XXX write error?

    delete impl->proc;
    impl->proc = 0;
    emit gapExited();
    emit gapStateChanged();
}

void Worksheet::gapScript(const QString &script)
{
    impl->script_proxy->runScript(script);
}

void Worksheet::gapOutput(const QString &text, GapProcess::OutputType type)
{
    if (type == GapProcess::Stderr)
        writeError(text);
    else
        writeOutput(text);
}

void Worksheet::gapMarkup(const QString &text)
{
    writeMarkup(text);
}

void Worksheet::gapPrompt(const QString &text, bool first_time)
{
    if (first_time)
    {
        setAcceptingInput(true);
        return;
    }

    bool continue_input = text == "> ";

    if (continue_input)
        continueInput();
    else
        startInput(text);
}

void WorksheetPrivate::forkCommand(const GapCommand &cmd)
{
    m_return_if_fail(proc == 0);
    m_return_if_fail(!cmd.args.isEmpty());

    proc = new GapProcess(cmd, width, height);

    M_Q(Worksheet);
    QObject::connect(proc, SIGNAL(started(GapProcess::StartResult, const QString&)), q,
                     SLOT(gapStarted(GapProcess::StartResult, const QString&)));
    QObject::connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), q,
                     SLOT(gapDied(int, QProcess::ExitStatus)));
    QObject::connect(proc, SIGNAL(error(int, QProcess::ExitStatus, const QString&)),
                     q, SLOT(gapError(int, QProcess::ExitStatus, const QString&)));

    QObject::connect(proc, SIGNAL(stateChanged()),
                     q, SIGNAL(gapStateChanged()));

    QObject::connect(proc, SIGNAL(output(const QString&, GapProcess::OutputType)),
                     q, SLOT(gapOutput(const QString&, GapProcess::OutputType)));
    QObject::connect(proc, SIGNAL(markup(const QString&)),
                     q, SLOT(gapMarkup(const QString&)));
    QObject::connect(proc, SIGNAL(prompt(const QString&, bool)),
                     q, SLOT(gapPrompt(const QString&, bool)));
    QObject::connect(proc, SIGNAL(globalsChanged(const char*, uint, GapProcess::GlobalsChange)),
                     q, SLOT(gapGlobalsChanged(const char*, uint, GapProcess::GlobalsChange)));
    QObject::connect(proc, SIGNAL(script(const QString&)),
                     q, SLOT(gapScript(const QString&)));

    proc->start();

    emit q->gapStateChanged();
}

void WorksheetPrivate::killGap()
{
    if (proc)
    {
        M_Q(Worksheet);
        proc->disconnect(q);
        proc->die();
        q->gapDied(0, QProcess::NormalExit);
    }
}


void Worksheet::interruptGap()
{
    if (impl->proc)
        impl->proc->sendIntr();
}

void Worksheet::restartGap()
{
    impl->killGap();
    impl->startGap();
}

namespace {
struct BusyCursor {
    BusyCursor() { QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); }
    ~BusyCursor() { QApplication::restoreOverrideCursor(); }
};
}

bool Worksheet::load(const QString &filename, QString &error)
{
    BusyCursor cursor;

    GwsReader gws(filename);
    if (!gws.unpack(error))
        return false;

    if (!loadXml(gws.worksheet(), error))
    {
        error = QString("loadXml failed: ") + error;
        return false;
    }

    QString workspace = gws.workspace();

    impl->file_type = workspace.isEmpty() ? File::Worksheet : File::Workspace;
    impl->killGap();
    impl->startGap(workspace);
    setAcceptingInput(false);
    setFilename(filename);
    setModified(false);

    return true;
}


static bool parse_save_workspace_output(const QString &output, QString &error)
{
    QStringList lines = output.split(QRegExp(QString::fromUtf8("\\r\\n|\\n|\\r|\xe2\x80\xa9")));

    foreach (const QString &p, lines)
    {
        if (p.startsWith('#'))
            continue;

        if (p == "true")
            return true;

        if (p.startsWith("Couldn't open file"))
            error = "could not save workspace";
        else
            error = QString("unknown output: '%1'").arg(p);

        return false;
    }

    error = "empty output";
    return false;
}

bool WorksheetPrivate::saveWorkspace(const QString &filename, QString &error)
{
    m_return_val_if_fail(proc, false);

    QString cmd = gapCmdSaveWorkspace(filename);

    QString output;
    bool result = proc->runCommand(GGAP_CMD_RUN_COMMAND, "", cmd, output);

    if (!result)
        error = "Failed";
    else
        result = parse_save_workspace_output(output, error);

    return result;
}

void Worksheet::save(const QString &filename)
{
    save(filename, impl->file_type);
}

void Worksheet::save(const QString &filename, File::Type type)
{
    setDocState(Saving);

    GapProcess::State gap_state = gapState();

    if (type == File::Workspace && gap_state != GapProcess::Dead && gap_state != GapProcess::InPrompt)
    {
        setDocState(Idle);
        emit savingFinished(false, "Can't save workspace when GAP is not in command prompt");
        return;
    }

    QString error;
    GwsWriter gws(filename);

    if (!gws.open(error))
    {
        setDocState(Idle);
        emit savingFinished(false, error);
        return;
    }

    bool save_workspace = type == File::Workspace;
    QString workspace;

    if (save_workspace && gap_state != GapProcess::Dead &&
        !impl->saveWorkspace(gws.workspaceFilename(), error))
    {
        setDocState(Idle);
        emit savingFinished(false, error);
        return;
    }

    BusyCursor cursor;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QByteArray data;
    QXmlStreamWriter xml(&data);
    format(xml);

    bool success = gws.addWorksheet(data, error) && gws.write(error);

    QApplication::restoreOverrideCursor();

    if (success)
    {
        setFilename(filename);
        setModified(false);
        impl->file_type = type;
    }

    setDocState(Idle);
    emit savingFinished(success, error);
}


/////////////////////////////////////////////////////////////////////////////
// Completion
//

namespace {
class Utf8LineReader {
    const char *ptr;
    const char *le;
    const char *next;
    const char *end;

    void get()
    {
        if (!le)
        {
            for (const char *p = ptr; p < end; ++p)
            {
                if (p[0] == '\n')
                {
                    le = p;
                    next = p + 1;
                    break;
                }
                else if (p[0] == '\r')
                {
                    le = p;
                    next = (p + 1 < end && p[1] == '\n') ? p + 2 : p + 1;
                    break;
                }
                /* Unicode paragraph separator "\xe2\x80\xa9" */
                else if (p[0] == '\xe2' && p + 2 < end && p[1] == '\x80' && p[2] == '\xa9')
                {
                    le = p;
                    next = p + 3;
                    break;
                }
            }

            if (!le)
            {
                le = end;
                next = 0;
            }
        }
    }

public:
    bool isNull() const
    {
        return !ptr;
    }

    void advance()
    {
        ptr = next;
        next = 0;
        le = 0;
    }

    QString getString()
    {
        get();
        return QString::fromUtf8(ptr, le - ptr);
    }

    Utf8LineReader(const char *data, uint size) :
        ptr(data), le(0), next(0), end(data + size)
    {
    }

    Utf8LineReader(const QByteArray &data) :
        ptr(data.constData()), le(0), next(0), end(data.constData() + data.size())
    {
    }
};
} // namespace

void WsCompleter::ensureModelSet()
{
    if (!model_set)
    {
        model_set = true;
        model.setStringList(items);
    }
}

void WsCompleter::unsetModel()
{
    QTimer::singleShot(0, this, SLOT(doUnsetModel()));
}

void WsCompleter::doUnsetModel()
{
    if (model_set)
    {
        model_set = false;
        model.setStringList(QStringList());
    }

    selection = QTextCursor();
}

void WsCompleter::clear()
{
    items = QStringList();
    model.setStringList(QStringList());
}

static void addSorted(QStringList &items, const QString &s)
{
    QStringList::iterator iter = std::lower_bound(items.begin(), items.end(), s);
    if (iter != items.end() && *iter == s)
        qCritical("%s: oops", Q_FUNC_INFO);
    else
        items.insert(iter, s);
}

void removeSorted(QStringList &items, const QString &s)
{
    QStringList::iterator iter = qBinaryFind(items.begin(), items.end(), s);
    if (iter == items.end())
        qCritical("%s: oops", Q_FUNC_INFO);
    else
        items.erase(iter);
}

void WsCompleter::addData(const char *data, uint size)
{
    QStringList new_items;

    for (Utf8LineReader lr(data, size); !lr.isNull(); lr.advance())
    {
        QString s = lr.getString();
        if (!s.isEmpty())
            new_items << s;
    }

    if (new_items.size() > 100)
    {
        items << new_items;
        items.sort();
    }
    else foreach (const QString &s, new_items)
    {
        addSorted(items, s);
    }
}

void WsCompleter::removeData(const char *data, uint size)
{
    // XXX it's latin1
    for (Utf8LineReader lr(data, size); !lr.isNull(); lr.advance())
        removeSorted(items, lr.getString());
}

void Worksheet::keyPressEvent(QKeyEvent *e)
{
    if (impl->completer.popup()->isVisible())
    {
        impl->completer.keyPressEvent(e);
        return;
    }

    if (e->key() == Qt::Key_Tab && e->modifiers() == 0 &&
        dynamic_cast<moo::ws::PromptBlock*>(blockAtCursor()))
    {
        impl->completer.maybeComplete();
        e->accept();
        return;
    }

    moo::ws::Worksheet::keyPressEvent(e);
}

WsCompleter::WsCompleter(Worksheet *ws) :
    QCompleter(),
    ws(ws),
    model(),
    model_set(false)
{
    connect(this, SIGNAL(activated(const QString&)),
            SLOT(doActivated(const QString&)));
    setModel(&model);
    setWidget(ws);
    setCompletionMode(PopupCompletion);
    setModelSorting(CaseSensitivelySortedModel);
    setCaseSensitivity(Qt::CaseSensitive);
    setWrapAround(false);
}

void Worksheet::gapGlobalsChanged(const char *data, uint size, GapProcess::GlobalsChange change)
{
    if (change == GapProcess::GlobalsAdded)
        impl->completer.addData(data, size);
    else
        impl->completer.removeData(data, size);
}

inline static bool isWordChar(const QString &s)
{
    QChar ch = s[0];
    return (ch.isLetterOrNumber() || ch == '_' || ch == '$') &&
                ch == ch.toAscii();
}

QString WsCompleter::getTextToComplete()
{
    M_FIXME();

    QTextCursor cr = ws->textCursor();

    if (cr.atBlockStart())
        return QString();

    QTextCursor start = cr;
    while (!start.atBlockStart())
    {
        start.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
        QString t = start.selectedText();
        if (!isWordChar(t))
        {
            start.movePosition(QTextCursor::NextCharacter);
            break;
        }
        start.setPosition(start.position());
    }

    if (start == cr)
        return QString();

    int cursor = cr.position();
    cr.setPosition(start.position());
    cr.setPosition(cursor, QTextCursor::KeepAnchor);
    selection = cr;
    return cr.selectedText();
}

void WsCompleter::doActivated(const QString &text)
{
    m_return_if_fail(!selection.isNull());
    ws->document().insertInteractive(selection, text, 0);
    ws->setTextCursor(selection);
    unsetModel();
}

static QString find_max_prefix(QCompleter *c, const QString &prefix)
{
    int total = c->completionCount();
    QString common = c->currentCompletion();
    for (int i = 1; i < total; ++i)
    {
        c->setCurrentRow(i);
        QString s = c->currentCompletion();

        if (s.length() <= prefix.length())
            return prefix;

        int j = prefix.length();
        for ( ; j < s.length() && j < common.length() && s[j] == common[j]; ++j)
            ;

        if (j == prefix.length())
            return prefix;

        common = common.left(j);
    }
    return common;
}

void WsCompleter::maybeComplete()
{
    m_return_if_fail(!popup()->isVisible());
    m_return_if_fail(!model_set);

    QString prefix = getTextToComplete();

    if (prefix.isEmpty())
    {
        ws->beep();
        return;
    }

    ensureModelSet();
    setCompletionPrefix(prefix);

    if (completionCount() == 0)
    {
        ws->beep();
        unsetModel();
        return;
    }

    if (completionCount() == 1)
    {
        if (currentCompletion() == prefix)
        {
            ws->beep();
            unsetModel();
        }
        else
        {
            emit activated(currentCompletion());
        }

        return;
    }

    QString new_prefix = find_max_prefix(this, prefix);
    if (new_prefix != prefix)
    {
        int start = selection.selectionStart();
        ws->document().insertInteractive(selection, new_prefix, 0);
        ws->setTextCursor(selection);
        int end = selection.position();
        selection.setPosition(start);
        selection.setPosition(end, QTextCursor::KeepAnchor);
        setCompletionPrefix(new_prefix);
        prefix = new_prefix;
    }

    popup()->setCurrentIndex(completionModel()->index(0, 0));

    QRect rect = ws->cursorRect(selection);
    rect.setWidth(popup()->sizeHintForColumn(0) +
                  popup()->verticalScrollBar()->sizeHint().width());
    complete(rect);
}

void WsCompleter::keyPressEvent(QKeyEvent *e)
{
    M_IMPLEMENT_ME();
    e->ignore();
}

bool WsCompleter::eventFilter(QObject *obj, QEvent *event)
{
    if (model_set && event->type() == QEvent::Hide && obj == popup())
        unsetModel();
    return QCompleter::eventFilter(obj, event);
}
