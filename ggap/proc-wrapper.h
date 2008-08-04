#ifndef GGAP_PROC_WRAPPER_H
#define GGAP_PROC_WRAPPER_H

#include "ggap/proc.h"
#include "moo-macros.h"
#include <QtCore>

namespace ggap {

template<typename T>
class RefPtr {
    T *p;
public:
    RefPtr(T *np = 0) : p(0) { *this = np; }
    ~RefPtr() { if (p) p->unref(); }
    RefPtr(const RefPtr &rp) : p(0) { *this = rp; }
    RefPtr &operator = (const RefPtr &rp) { *this = rp.p; return *this; }
    RefPtr &operator = (T *np) { if (np != p) {if (p) p->unref(); p = np; if (p) p->ref(); } return *this; }
    operator bool () const { return p != 0; }
    operator T* () { return p; }
    T *operator -> () { return p; }
    T &operator * () { return *p; }
};

class ProcessReaper : public QProcess {
    Q_OBJECT

    uint ref_count;
    RefPtr<ProcessReaper> self;

    void start(const QString &program, OpenMode mode = ReadWrite);

    ~ProcessReaper()
    {
    }

public:
    ProcessReaper(QObject *parent = 0) :
        QProcess(parent),
        ref_count(0)
    {
    }

    void ref()
    {
        ++ref_count;
    }

    void unref()
    {
        m_return_if_fail(ref_count != 0);
        if (!--ref_count)
            deleteLater();
    }

    void start(const QString &program, const QStringList &arguments, OpenMode mode = ReadWrite)
    {
        QProcess::start(program, arguments, mode);
        self = this;
        connect(this, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(error(QProcess::ProcessError)));
        connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(finished(int, QProcess::ExitStatus)));
    }

private Q_SLOTS:
    void error(QProcess::ProcessError error)
    {
        if (error == QProcess::FailedToStart)
            self = 0;
    }

    void finished(int, QProcess::ExitStatus)
    {
        self = 0;
    }
};

struct GapMsg {
    enum Type {
        Text = 1,
        Status,
        Reply,
        Prompt,
        Script,
        Markup,
        Globals
    };

    Type type;
    QByteArray data;
    QVariant extra;

    static GapMsg text(const char *data, uint len, GapProcess::OutputType type);
    static GapMsg text(const QByteArray &data, GapProcess::OutputType type);
    static GapMsg parseData(const QByteArray &data);

    QString toString() const;
    GapProcess::OutputType outputType() const;
    GapProcess::GlobalsChange globalsChange() const;

private:
    GapMsg(Type type, const char *data, uint len);
    GapMsg(Type type, const QByteArray &data);
    GapMsg(Type type, const QByteArray &data, uint offset);
    GapMsg(Type type, const QByteArray &data, uint offset, const QVariant &extra);
};

inline QDebug &operator<<(QDebug debug, const GapMsg &msg)
{
    debug.nospace() << "{ GapMsg: ";
    switch (msg.type)
    {
        case GapMsg::Text:
            debug << "Text<" << msg.data << ">}";
            break;
        case GapMsg::Status:
            debug << "Status<" << msg.data << ">}";
            break;
        case GapMsg::Reply:
            debug << "Reply<" << msg.data << ">}";
            break;
        case GapMsg::Prompt:
            debug << "Prompt<" << msg.data << ">}";
            break;
        case GapMsg::Script:
            debug << "Script<" << msg.data << ">}";
            break;
        case GapMsg::Markup:
            debug << "Markup<" << msg.data << ">}";
            break;
        case GapMsg::Globals:
            debug << "Globals>}";
            break;
    }
    return debug.space();
}

class GapProcessParser {
    enum InputState {
        InputNormal,
        InputMaybeMagic,
        InputDataType,
        InputDataFixedLen,
        InputDataFixed,
        InputDataVar,
        InputMaybeDataEnd
    };

    QByteArray inputBuf;
    QByteArray inputBuf2;
    uint inputDataLen;
    InputState inputState;
    GapProcess::OutputType type;
    QList<GapMsg> messages;

    void readCharsNormal(char const **data_p, uint *data_len_p);
    void readCharsMaybeMagic(char const **data_p, uint *data_len_p);
    void readCharsDataType(char const **data_p, uint *data_len_p);
    void readCharsDataFixedLen(char const **data_p, uint *data_len_p);
    void readCharsDataFixed(char const **data_p, uint *data_len_p);
    void readCharsDataInf(char const **data_p, uint *data_len_p);
    void readCharsMaybeDataEnd(char const **data_p, uint *data_len_p);

public:
    GapProcessParser(GapProcess::OutputType type) :
        inputDataLen(0), inputState(InputNormal), type(type)
    {
    }

    void process_data(const char **buf, uint *len, QList<GapMsg> &dest);
};

class GapProcessWrapper : public QObject {
    Q_OBJECT

    QStringList args;
    QList<GapMsg> output;
    RefPtr<ProcessReaper> proc;
    GapProcessParser parser_stdout, parser_stderr;
    QTimer *timer;
    bool gap_started;

    void start()
    {
        qDebug() << "starting GAP: " << args;

        gap_started = false;
        proc = new ProcessReaper;

        connect(proc, SIGNAL(started()), SLOT(childStarted()));
        connect(proc, SIGNAL(error(QProcess::ProcessError)),
                SLOT(childError(QProcess::ProcessError)));
        connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
                SLOT(childDied(int, QProcess::ExitStatus)));

        connect(proc, SIGNAL(readyReadStandardError()),
                SLOT(readyReadStandardError()));
        connect(proc, SIGNAL(readyReadStandardOutput()),
                SLOT(readyReadStandardOutput()));

        if (1)
            // otherwise stderr gets mixed with stdout
            proc->setProcessChannelMode(QProcess::MergedChannels);

        timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->start(1000);
        connect(timer, SIGNAL(timeout()), SLOT(timeout()));

        proc->start(args.at(0), args.mid(1), QIODevice::ReadWrite | QIODevice::Unbuffered);
    }

    void disconnectTimer()
    {
        if (timer)
        {
            timer->disconnect(this);
            timer->deleteLater();
            timer = 0;
        }
    }

    void disconnectProcess()
    {
        if (proc)
        {
            proc->disconnect(this);
            proc = 0;
        }
    }

    void successStarting()
    {
        qDebug() << Q_FUNC_INFO;
        disconnectTimer();
        gap_started = true;
        emit started();
        dispatchOutput(output);
    }

    void errorStarting(int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << Q_FUNC_INFO;
        disconnectProcess();
        emit error(exitCode, exitStatus, output);
    }

    void dispatchOutput(const QList<GapMsg> &messages)
    {
        if (messages.isEmpty())
            return;

        if (!gap_started)
        {
            bool started = false;
            foreach (const GapMsg &msg, messages)
            {
                if (msg.type == GapMsg::Status && msg.toString() == "started")
                {
                    started = true;
                    break;
                }
            }

            if (started)
            {
                qDebug() << Q_FUNC_INFO << "got status:started";
                successStarting();
            }
        }

        if (!gap_started)
            output << messages;
        else foreach (const GapMsg &msg, messages)
            emit message(msg);
    }

private Q_SLOTS:
    void timeout()
    {
        qDebug() << Q_FUNC_INFO;
        // If we got here, then the process is still alive. Emit started()
        successStarting();
    }

    void childStarted()
    {
        // This means GAP did start but it may yet spit out error messages
        // and exit. Wait.
        qDebug() << Q_FUNC_INFO;
    }

    void childError(QProcess::ProcessError error)
    {
        qDebug() << Q_FUNC_INFO << error;

        if (!gap_started)
        {
            // died unborn, report
            errorStarting(-1, QProcess::CrashExit);
        }
        else
        {
            // normal crash
            disconnectProcess();
            emit finished(-1, QProcess::CrashExit);
        }
    }

    void childDied(int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << Q_FUNC_INFO << exitCode << exitStatus;

        if (!gap_started)
        {
            // It died before the timeout, probably printed errors and quit.
            errorStarting(exitCode, exitStatus);
        }
        else
        {
            // Normal termination, report.
            disconnectProcess();
            emit finished(exitCode, exitStatus);
        }
    }

private:
    void childOutput(const QByteArray &data, GapProcess::OutputType type)
    {
        GapProcessParser &parser = type == GapProcess::Stdout ?
                                    parser_stdout : parser_stderr;

//         if (type == GapProcess::Stderr)
//             write_log(data.data(), data.size(), true);

        const char *buf = data.data();
        uint len = data.size();
        QList<GapMsg> messages;

        while (len)
            parser.process_data(&buf, &len, messages);

        if (!messages.isEmpty())
            dispatchOutput(messages);
    }

private Q_SLOTS:
    void readyReadStandardOutput()
    {
        childOutput(proc->readAllStandardOutput(), GapProcess::Stdout);
    }

    void readyReadStandardError()
    {
        childOutput(proc->readAllStandardError(), GapProcess::Stderr);
    }

public:
    GapProcessWrapper(QObject *parent) :
        QObject(parent),
        parser_stdout(GapProcess::Stdout),
        parser_stderr(GapProcess::Stderr),
        timer(0), gap_started(false)
    {
    }

    ~GapProcessWrapper() {}

    void start(const QStringList &cmd)
    {
        args = cmd;
        qDebug() << Q_FUNC_INFO;
        start();
    }

    void write(const QByteArray &data)
    {
        m_return_if_fail(gap_started);
        m_return_if_fail(proc);
//         write_log(data, -1, false);
        proc->write(data);
    }

    void kill()
    {
        RefPtr<ProcessReaper> p = proc;
        disconnectProcess();
        if (p)
            p->kill();
    }

    void sendIntr();

Q_SIGNALS:
    // After start() is called, exactly one of started() or error()
    // is emitted. started() means GAP is running, and finished() will
    // be emitted eventually when GAP quits (no matter what QProcess
    // reports, finished() or error());
    // error() means GAP did not start
    void started();
    void error(int exitCode, QProcess::ExitStatus exitStatus, const QList<GapMsg> &output);

    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void message(const GapMsg &msg);
};

class GapProcessWrapper2 : public QObject {
    Q_OBJECT

    GapProcessWrapper *proc;
    QStringList next_args;
    QList<GapMsg> output;
    bool first_time;
    bool running;

    void start(const QStringList &args)
    {
        qDebug() << Q_FUNC_INFO;
        M_ASSERT(!proc);
        proc = new GapProcessWrapper(this);
        connect(proc, SIGNAL(started()), SLOT(childStarted()));
        connect(proc, SIGNAL(error(int,QProcess::ExitStatus,const QList<GapMsg>&)),
                SLOT(childError(int,QProcess::ExitStatus,const QList<GapMsg>&)));
        proc->start(args);
    }

private Q_SLOTS:
    void childStarted()
    {
        running = true;

        connect(proc, SIGNAL(message(const GapMsg&)),
                SIGNAL(message(const GapMsg&)));
        connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
                SIGNAL(finished(int, QProcess::ExitStatus)));

        if (first_time)
            emit started(true, QList<GapMsg>());
        else
            emit started(false, output);
    }

    void childError(int exitCode, QProcess::ExitStatus exitStatus,
                    const QList<GapMsg> &errorOutput)
    {
        proc->disconnect(this);
        proc->deleteLater();
        proc = 0;

        if (first_time && !next_args.isEmpty())
        {
            first_time = false;
            output = errorOutput;

            qDebug() << "first invocation failed:" << output;

            QStringList args;
            qSwap(args, next_args);
            start(args);
        }
        else
        {
            emit error(exitCode, exitStatus, errorOutput);
        }
    }

public:
    GapProcessWrapper2(QObject *parent) :
        QObject(parent),
        proc(0),
        first_time(true),
        running(false)
    {
    }

    ~GapProcessWrapper2()
    {
    }

    void start(const GapCommand &cmd)
    {
        next_args = cmd.args2;
        first_time = true;
        running = false;
        start(cmd.args);
    }

    void write(const QByteArray &data)
    {
        m_return_if_fail(proc && running);
        proc->write(data);
    }

    void kill()
    {
        if (proc)
        {
            proc->disconnect(this);
            proc->deleteLater();
            proc = 0;
        }
    }

    void sendIntr()
    {
        m_return_if_fail(proc && running);
        proc->sendIntr();
    }

Q_SIGNALS:
    // After start() is called, exactly one of started() or error()
    // is emitted. started() means GAP is running, and finished() will
    // be emitted eventually when GAP quits; error() means GAP did not
    // start.
    void started(bool success, const QList<GapMsg> &output);
    void error(int exitCode, QProcess::ExitStatus exitStatus, const QList<GapMsg> &output);

    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void message(const GapMsg &msg);
};

} // namespace ggap

#endif // GGAP_PROC_WRAPPER_H
