#ifndef GGAP_PROC_H
#define GGAP_PROC_H

#include <QProcess>
#include <QStringList>
#include <moo-pimpl.h>

namespace ggap {

struct GapCommand {
    enum Flag {
        UserWorkspace = 1 << 0
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    Flags flags;
    QStringList args;
    QStringList args2;

    GapCommand()
    {
    }

    GapCommand(Flags flags, const QStringList args, const QStringList args2) :
        flags(flags), args(args), args2(args2)
    {
    }
};

class GapProcessPrivate;
class GapProcess : public QObject {
    Q_OBJECT
    Q_ENUMS(State OutputType GlobalsChange)
    M_DECLARE_IMPL(GapProcess)
    GapCommand cmd;
    int width;
    int height;

public:
    enum State {
        Dead = 0,
        Starting,
        Loading,
        Busy,
        InPrompt
    };

    enum OutputType {
        Stdout,
        Stderr
    };

    enum GlobalsChange {
        GlobalsAdded,
        GlobalsRemoved
    };

    enum StartResult {
        StartedSuccessfully,
        AutoWorkspaceFailed,
        UserWorkspaceFailed
    };

Q_SIGNALS:
    void stateChanged();

    void started(GapProcess::StartResult result, const QString &output);
    void error(int exitCode, QProcess::ExitStatus exitStatus, const QString &output);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

    void script(const QString &script);
    void prompt(const QString &prompt, bool first_time);
    void globalsChanged(const char *data, uint data_len, GapProcess::GlobalsChange change);

    void output(const QString &text, GapProcess::OutputType type);
    void markup(const QString &string);

public:
    GapProcess(const GapCommand &cmd, int width, int height);
    ~GapProcess();

    void start();

    State state() const;

    void setWindowSize(int width, int height);

    void sendIntr();
    void writeInput(const QStringList &lines, bool askCompletions = false);
    bool runCommand(const QString &command,
                    const QString &args,
                    const QString &gap_cmd_line,
                    QString       &output);
    void askCompletions();

    void die();
};

inline QString gapOutputToString(const char *data, uint data_len)
{
    return QString::fromLatin1(data, data_len);
}

inline QString gapOutputToString(const QByteArray &data)
{
    return gapOutputToString(data.data(), data.size());
}

} // namespace ggap

#endif // GGAP_PROC_H
