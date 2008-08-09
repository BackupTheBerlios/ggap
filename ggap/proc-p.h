#ifndef GGAP_PROC_P_H
#define GGAP_PROC_P_H

#include "ggap/proc-wrapper.h"
#include "ggap/gap.h"
#include "moo-macros.h"
#include <QtCore>

namespace ggap {

class GapProcessPrivate : public QObject {
    Q_OBJECT
    M_DECLARE_PUBLIC(GapProcess)

    struct CommandInfo {
        QEventLoop *loop;
        bool success;
        bool destroyed;
        QString output;
        bool finished;
    };

    GapCommand args;
    GapProcessWrapper2 *proc;
    GapProcess::State state;
    CommandInfo *cmdInfo;
    bool loaded;

    void setState(GapProcess::State newState)
    {
        if (newState != state)
        {
            state = newState;
            M_Q(GapProcess);
            emit q->stateChanged();
        }
    }

    void write_log (const char *text,
                    int         len,
                    bool        in)
    {
//     MOO_DEBUG({
        static int last = -1;

        if (last != in)
        {
            if (last != -1)
                printf("\n");

            if (!in)
                printf("<-- ");
            else
                printf("--> ");
        }

        last = in;

        if (len < 0)
            len = strlen(text);

        while (len)
        {
            int i;

            for (i = 0; i < len && text[i] != '\n'; ++i) ;

            printf("%.*s", int(i), text);

            if (i != len)
            {
                if (!in)
                    printf("\n<-- ");
                else
                    printf("\n--> ");
            }

            if (i < len)
            {
                len -= i + 1;
                text += i + 1;
            }
            else
            {
                len = 0;
                text += len;
            }
        }

        fflush(stdout);
//     }); /* MOO_DEBUG */
    }

    void writeChild(const QString &text)
    {
        m_return_if_fail(proc);
        QByteArray bytes = text.toLatin1();
        write_log(bytes, -1, false);
        proc->write(bytes);
    }

    void stopRunningCommandLoop(CommandInfo *ci);
    bool hasRunningCommandLoop();

    void doOutput(const QByteArray &output, GapProcess::OutputType type);
    void doMarkup(const QString &text);
    void doScript(const QString &text);
    void doPrompt(const QString &text);
    void doReply(const QString &text);
    void doGlobals(const QByteArray &data, GapProcess::GlobalsChange change);

private Q_SLOTS:
    void childFinished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        if (proc)
        {
            proc->deleteLater();
            proc = 0;
        }

        setState(GapProcess::Dead);

        M_Q(GapProcess);
        emit q->finished(exitCode, exitStatus);
    }

    void childStarted(bool success, const QList<GapMsg> &output)
    {
        setState(GapProcess::Loading);

        M_Q(GapProcess);
        QString string;
        foreach (const GapMsg &m, output)
            if (m.type == GapMsg::Text)
                string += m.toString();

        GapProcess::StartResult result;
        if (success)
            result = GapProcess::StartedSuccessfully;
        else if (args.flags & GapCommand::UserWorkspace)
            result = GapProcess::UserWorkspaceFailed;
        else
            result = GapProcess::AutoWorkspaceFailed;

        emit q->started(result, string);
    }

    void childError(int exitCode, QProcess::ExitStatus exitStatus, const QList<GapMsg> &output)
    {
        setState(GapProcess::Dead);

        M_Q(GapProcess);
        QString string;
        foreach (const GapMsg &m, output)
            if (m.type == GapMsg::Text)
                string += m.toString();
        emit q->error(exitCode, exitStatus, string);
    }

    void message(const GapMsg &msg)
    {
        switch (msg.type)
        {
            case GapMsg::Text:
                doOutput(msg.data, msg.outputType());
                break;
            case GapMsg::Status:
                // XXX pass it to worksheet?
                break;
            case GapMsg::Reply:
                doReply(msg.toString());
                break;
            case GapMsg::Prompt:
                doPrompt(msg.toString());
                break;
            case GapMsg::Script:
                doScript(msg.toString());
                break;
            case GapMsg::Markup:
                doMarkup(msg.toString());
                break;
            case GapMsg::Globals:
                doGlobals(msg.data, msg.globalsChange());
                break;
        }
    }

public:
    GapProcessPrivate(GapProcess *p) :
        pub(p),
        proc(0),
        state(GapProcess::Dead),
        cmdInfo(0), loaded(false)
    {
    }

    ~GapProcessPrivate()
    {
        die();
    }

    void start(const GapCommand &cmd, int, int)
    {
        m_return_if_fail(!proc);

        args = cmd;
        setState(GapProcess::Starting);

        proc = new GapProcessWrapper2(this);
        connect(proc, SIGNAL(started(bool, const QList<GapMsg>&)),
                SLOT(childStarted(bool, const QList<GapMsg>&)));
        connect(proc, SIGNAL(error(int, QProcess::ExitStatus, const QList<GapMsg>&)),
                SLOT(childError(int, QProcess::ExitStatus, const QList<GapMsg>&)));
        connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
                SLOT(childFinished(int, QProcess::ExitStatus)));
        connect(proc, SIGNAL(message(const GapMsg&)),
                SLOT(message(const GapMsg&)));
        proc->start(cmd);
    }

    void setWindowSize(int, int)
    {
        if (proc)
        {
            // moo_term_pt_set_size (pt, width, height);
            // ??? XXX
        }
    }

    void sendIntr()
    {
        if (state == GapProcess::Busy || state == GapProcess::Loading)
        {
            m_return_if_fail(proc);
            proc->sendIntr();
        }
    }

    void writeInput(const QStringList &lines, bool askCompletions = false)
    {
        m_return_if_fail(!lines.isEmpty());

        if (lines.size() > 1)
            writeChild(lines.join(" "));
        else
            writeChild(lines.at(0));

        writeChild("\n");

        if (askCompletions)
            writeChild(gapCmdRunCommand("get-globals"));

        setState(GapProcess::Busy);
    }

    bool runCommand(const QString &command,
                    const QString &args,
                    const QString &gap_cmd_line,
                    QString       &output);

    void askCompletions();

    void die()
    {
        if (cmdInfo)
        {
            cmdInfo->destroyed = true;
            stopRunningCommandLoop(cmdInfo);
        }

        if (proc)
        {
            proc->disconnect(this);
            proc->kill();
            proc = 0;
        }
    }
};

} // namespace ggap

#endif // GGAP_PROC_P_H
