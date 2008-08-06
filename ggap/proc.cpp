#include "ggap/proc-p.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifndef Q_OS_WIN32
#include <signal.h>
#else
#include <windows.h>
#endif

using namespace ggap;

// #ifdef Q_OS_WIN32
//
// namespace {
//
// struct FindWindowData {
//     DWORD proc_id;
//     HWND handle;
// };
//
// static BOOL find_window_func(HWND hwnd, LPARAM lParam)
// {
//     FindWindowData *data = lParam;
//     DWORD proc_id = 0;
//
//     GetWindowThreadProcessId(hwnd, &proc_id);
//
//     if (proc_id == data->proc_id)
//     {
//         data->handle = hwnd;
//         return FALSE;
//     }
//
//     return TRUE;
// }
//
// }
//
// #endif

void GapProcessWrapper::sendIntr()
{
#ifndef Q_OS_WIN32
    ::kill(process->pid(), SIGINT);
#else

    PROCESS_INFORMATION *pi = (PROCESS_INFORMATION*) process->pid();
//     FindWindowData data = { pi->dwProcessId, 0 };
//     EnumWindows(find_window_func, LPARAM lParam);
//
//     if (!data.handle)
//     {
//         qCritical("could not find window for process id %d",
//                   (int) pi->dwProcessId);
//         return;
//     }

    qDebug("sending Ctrl-C to process id %d", (int) pi->dwProcessId);
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, pi->dwProcessId);

#endif // Q_OS_WIN32
}

///////////////////////////////////////////////////////////////////////////
//
// GapMsg
//
GapMsg::GapMsg(Type type, const char *data, uint len) :
    type(type), data(data, len)
{
}

GapMsg::GapMsg(Type type, const QByteArray &data) :
    type(type), data(data)
{
}

GapMsg GapMsg::text(const char *data, uint len, GapProcess::OutputType type)
{
    GapMsg msg(Text, data, len);
    msg.extra = int(type);
    return msg;
}

GapMsg GapMsg::text(const QByteArray &data, GapProcess::OutputType type)
{
    GapMsg msg(Text, data);
    msg.extra = int(type);
    return msg;
}

GapMsg::GapMsg(Type type, const QByteArray &data, uint offset) :
    type(type), data(data.data() + offset, data.size() - offset)
{
}

GapMsg::GapMsg(Type type, const QByteArray &data, uint offset, const QVariant &extra) :
    type(type), data(data.data() + offset, data.size() - offset), extra(extra)
{
}

GapMsg GapMsg::parseData(const QByteArray &data)
{
    if (data.startsWith("prompt:"))
        return GapMsg(Prompt, data, strlen("prompt:"));
    else if (data.startsWith("output:"))
        return GapMsg(Reply, data, strlen("output:"));
    else if (data.startsWith("script:"))
        return GapMsg(Script, data, strlen("script:"));
    else if (data.startsWith("markup:"))
        return GapMsg(Markup, data, strlen("markup:"));
    else if (data.startsWith("status:"))
        return GapMsg(Status, data, strlen("status:"));
    else if (data.startsWith("globals-added:"))
        return GapMsg(Globals, data, strlen("globals-added:"),
                      GapProcess::GlobalsAdded);
    else if (data.startsWith("globals-deleted:"))
        return GapMsg(Globals, data, strlen("globals-deleted:"),
                      GapProcess::GlobalsRemoved);

    qCritical ("%s: got unknown data: '%s'", Q_FUNC_INFO, data.data());
    return GapMsg(Text, data);
}

QString GapMsg::toString() const
{
    return gapOutputToString(data);
}

GapProcess::OutputType GapMsg::outputType() const
{
    m_return_val_if_fail(type == Text, GapProcess::Stdout);
    return GapProcess::OutputType(extra.toInt());
}

GapProcess::GlobalsChange GapMsg::globalsChange() const
{
    m_return_val_if_fail(type == Globals, GapProcess::GlobalsAdded);
    return GapProcess::GlobalsChange(extra.toInt());
}


///////////////////////////////////////////////////////////////////////////
//
// GapProcessParser
//

#define MAGIC       "@GGAP@"
#define MAGIC_LEN   6u
#define LENGTH_LEN  8u

void GapProcessParser::readCharsNormal(char const **data_p, uint *data_len_p)
{
    const char *data = *data_p;
    uint data_len = *data_len_p;

    M_ASSERT(inputBuf.isEmpty());

    for (uint i = 0; i < data_len; ++i)
    {
        if (data[i] == '@')
        {
            uint n = qMin(MAGIC_LEN, data_len - i);

            if (!n || strncmp (data + i, MAGIC, n) == 0)
            {
                if (i > 0)
                    messages << GapMsg::text(data, i, type);

                if (n == MAGIC_LEN)
                {
                    inputState = InputDataType;
                }
                else
                {
                    inputState = InputMaybeMagic;
                    inputBuf = QByteArray(data + i, n);
                }

                *data_p = data + i + n;
                *data_len_p = data_len - i - n;

                return;
            }
        }
    }

    messages << GapMsg::text(data, data_len, type);
    *data_p = data + data_len;
    *data_len_p = 0;
}

inline static void append(QByteArray &array, const char *data, uint len)
{
    // array.append() crashes on qt-4.3
    uint old_size = array.size();
    array.resize(old_size + len);
    memcpy(array.data() + old_size, data, len);
}

inline static void erase(QByteArray &array)
{
    array.resize(0);
    array.squeeze();
}

void GapProcessParser::readCharsMaybeMagic(char const **data_p, uint *data_len_p)
{
    const char *data = *data_p;
    uint data_len = *data_len_p;
    uint n;

    M_ASSERT((int) MAGIC_LEN < inputBuf.size());
    n = qMin(data_len, MAGIC_LEN - inputBuf.size());
    append(inputBuf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (strncmp (inputBuf.data(), MAGIC, inputBuf.size()) != 0)
    {
        QByteArray tmp;
        qSwap(inputBuf, tmp);
        inputState = InputNormal;
        messages << GapMsg::text(tmp, type);
    }
    else if (inputBuf.size() == MAGIC_LEN)
    {
        erase(inputBuf);
        inputState = InputDataType;
    }
}

void GapProcessParser::readCharsDataType(char const **data_p, uint *data_len_p)
{
    const char *data = *data_p;
    uint data_len = *data_len_p;

    M_ASSERT(inputState == InputDataType);
    M_ASSERT(inputBuf.size() == 0);

    m_return_if_fail(data_len != 0);

    *data_p = data + 1;
    *data_len_p = data_len - 1;

    switch (data[0])
    {
        case GGAP_DTC_FIXED:
            inputState = InputDataFixedLen;
            break;
        case GGAP_DTC_VARIABLE:
            inputState = InputDataVar;
            break;
        default:
            qCritical("%s: unknown data type '%c'", Q_FUNC_INFO, data[0]);
            inputState = InputNormal;
            break;
    }
}

static uint
get_length (const char *str)
{
    ulong n;
    char *end;

    errno = 0;
    n = strtoul (str, &end, 16);

    if (errno != 0 || end == NULL || *end != 0)
    {
        qCritical("%s: could not convert '%s' to a number", Q_FUNC_INFO, str);
        return 0;
    }

    return n;
}

void GapProcessParser::readCharsDataFixedLen(char const **data_p, uint *data_len_p)
{
    const char *data = *data_p;
    uint data_len = *data_len_p;
    uint n;

    M_ASSERT(inputBuf.size() < int(LENGTH_LEN));

    n = qMin(data_len, LENGTH_LEN - inputBuf.size());
    append(inputBuf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (inputBuf.size() == LENGTH_LEN)
    {
        uint input_data_len = get_length(inputBuf.data());

        if (input_data_len == 0)
        {
            inputState = InputNormal;
            erase(inputBuf);
        }
        else
        {
            erase(inputBuf);
            inputState = InputDataFixed;
            inputDataLen = input_data_len;
        }
    }
}

void GapProcessParser::readCharsDataFixed(char const **data_p, uint *data_len_p)
{
    const char *data = *data_p;
    uint data_len = *data_len_p;
    uint remain = inputDataLen - inputBuf.size();
    uint n;

    M_ASSERT((int) inputDataLen > inputBuf.size());
    n = qMin(remain, data_len);
    append(inputBuf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (inputBuf.size() == int(inputDataLen))
    {
        QByteArray tmp;
        qSwap(tmp, inputBuf);
        inputDataLen = 0;
        inputState = InputNormal;
        messages << GapMsg::parseData(tmp);
    }
}

void GapProcessParser::readCharsDataInf(char const **data_p, uint *data_len_p)
{
    const char *data = *data_p;
    uint data_len = *data_len_p;
    uint i;

//     M_ASSERT(inputBuf.size() != 0);
    M_ASSERT(inputBuf2.size() == 0);

    for (i = 0; i < data_len; ++i)
    {
        if (data[i] == '@')
        {
            uint n = qMin(MAGIC_LEN, data_len - i);

            if (!n || strncmp (data + i, MAGIC, n) == 0)
            {
                if (i > 0)
                    append(inputBuf, data, i);

                inputState = InputMaybeDataEnd;
                inputBuf2 = QByteArray(data + i, n);

                *data_p = data + i + n;
                *data_len_p = data_len - i - n;

                return;
            }
        }
    }

    append(inputBuf, data, data_len);
    *data_p = data + data_len;
    *data_len_p = 0;
}

void GapProcessParser::readCharsMaybeDataEnd(char const **data_p, uint *data_len_p)
{
    const char *data = *data_p;
    uint data_len = *data_len_p;

    M_ASSERT(inputBuf.size() != 0);
    M_ASSERT(inputBuf2.size() != 0);

    if (inputBuf2.size() < int(MAGIC_LEN))
    {
        uint n = qMin(data_len, MAGIC_LEN - inputBuf2.size());

        append(inputBuf2, data, n);
        data += n;
        data_len -= n;
        *data_p = data;
        *data_len_p = data_len;

        if (inputBuf2.size() < int(MAGIC_LEN))
            return;

        if (strncmp (inputBuf2.data(), MAGIC, inputBuf2.size()) != 0)
        {
            inputBuf.append(inputBuf2);
            erase(inputBuf2);
            inputState = InputDataVar;
            return;
        }
    }

    if (data_len == 0)
        return;

    *data_p = data + 1;
    *data_len_p = data_len - 1;

    if (data[0] == GGAP_DTC_END)
    {
        QByteArray tmp;
        qSwap(tmp, inputBuf2);
        tmp = inputBuf;
        inputBuf = QByteArray();
        inputState = InputNormal;
        messages << GapMsg::parseData(tmp);
    }
    else
    {
        qDebug("%s: got '%c', expecting '%c'",
               Q_FUNC_INFO, data[0], GGAP_DTC_END);
        inputBuf.append(inputBuf2);
        inputBuf.append(data[0]);
        inputState = InputDataVar;
        erase(inputBuf2);
    }
}

void GapProcessParser::process_data(const char **buf, uint *len, QList<GapMsg> &dest)
{
    switch (inputState)
    {
        case InputNormal:
            readCharsNormal(buf, len);
            break;

        case InputMaybeMagic:
            readCharsMaybeMagic(buf, len);
            break;
        case InputDataType:
            readCharsDataType(buf, len);
            break;

        case InputDataFixedLen:
            readCharsDataFixedLen(buf, len);
            break;
        case InputDataFixed:
            readCharsDataFixed(buf, len);
            break;

        case InputDataVar:
            readCharsDataInf(buf, len);
            break;
        case InputMaybeDataEnd:
            readCharsMaybeDataEnd(buf, len);
            break;
    }

    if (!messages.isEmpty())
    {
        dest << messages;
        messages.clear();
    }
}


///////////////////////////////////////////////////////////////////////////
//
// GapProcess
//

void GapProcessPrivate::doOutput(const QByteArray &output, GapProcess::OutputType type)
{
    const char *data = output.data();
    uint data_len = output.size();

    m_return_if_fail(data_len != 0);

    while (data_len)
    {
        uint i = 0;

        while (i < data_len && data[i] != '\003')
            i++;

        if (i != 0)
        {
            if (hasRunningCommandLoop())
            {
                qCritical("%s: oops", Q_FUNC_INFO);
                stopRunningCommandLoop(cmdInfo);
            }

            if (loaded)
            {
                M_Q(GapProcess);
                emit q->output(gapOutputToString(data, i), type);
            }
        }

        while (i < data_len && data[i] == '\003')
            i++;

        data += i;
        data_len -= i;
    }
}

void GapProcessPrivate::doMarkup(const QString &text)
{
    if (hasRunningCommandLoop())
    {
        qCritical("%s: oops", Q_FUNC_INFO);
        stopRunningCommandLoop(cmdInfo);
    }

    M_Q(GapProcess);
    emit q->markup(text);
}

void GapProcessPrivate::doPrompt(const QString &text)
{
    if (hasRunningCommandLoop())
    {
        qCritical("%s: oops", Q_FUNC_INFO);
        stopRunningCommandLoop(cmdInfo);
    }

    bool was_loaded = loaded;
    loaded = true;

    M_Q(GapProcess);
    emit q->prompt(text, !was_loaded);
    setState(GapProcess::InPrompt);
}

void GapProcessPrivate::doReply(const QString &text)
{
    if (hasRunningCommandLoop())
    {
        cmdInfo->output = text;
        cmdInfo->success = true;
        stopRunningCommandLoop(cmdInfo);
    }
    else
    {
        qCritical("%s: output: %s", Q_FUNC_INFO, qPrintable(text));
    }
}

void GapProcessPrivate::doScript(const QString &text)
{
    M_Q(GapProcess);
    emit q->script(text);
}

void GapProcessPrivate::doGlobals(const QByteArray &data, GapProcess::GlobalsChange change)
{
    M_Q(GapProcess);
    emit q->globalsChanged(data.data(), data.size(), change);
}

void GapProcessPrivate::stopRunningCommandLoop(CommandInfo *ci)
{
    if (ci->loop && ci->loop->isRunning())
        ci->loop->exit();
}

bool GapProcessPrivate::hasRunningCommandLoop()
{
    return cmdInfo && cmdInfo->loop && cmdInfo->loop->isRunning();
}

bool GapProcessPrivate::runCommand(const QString &command,
                                   const QString &args,
                                   const QString &gap_cmd_line,
                                   QString       &output)
{
    CommandInfo ci;

    m_return_val_if_fail(cmdInfo == NULL, false);
    m_return_val_if_fail(!command.isEmpty(), false);

    writeChild(gapCmdRunCommand(command, args));

    if (!gap_cmd_line.isEmpty())
        writeChild(gap_cmd_line);

    cmdInfo = &ci;
    ci.success = false;
    ci.loop = 0;
    ci.destroyed = false;
    ci.output = QString();

    QPointer<GapProcessPrivate> self_ptr = this;

    ci.loop = new QEventLoop;
    ci.loop->exec();
    ci.loop = NULL;

    if (ci.destroyed)
        ci.success = false;

    output = ci.output;

    if (self_ptr)
        cmdInfo = NULL;

    return ci.success;
}


void GapProcessPrivate::askCompletions()
{
    if (state == GapProcess::InPrompt)
        writeChild(gapCmdRunCommand("get-globals"));
}


GapProcess::GapProcess(const GapCommand &cmd, int width, int height) :
    impl(this), cmd(cmd), width(width), height(height)
{
}

GapProcess::~GapProcess()
{
}

void GapProcess::start()
{
    impl->start(cmd, width, height);
}

GapProcess::State GapProcess::state() const
{
    return impl->state;
}

void GapProcess::setWindowSize(int width, int height)
{
    impl->setWindowSize(width, height);
}

void GapProcess::writeInput(const QStringList &lines, bool askCompletions)
{
    impl->writeInput(lines, askCompletions);
}

void GapProcess::sendIntr()
{
    impl->sendIntr();
}

bool GapProcess::runCommand(const QString &command,
                            const QString &args,
                            const QString &gap_cmd_line,
                            QString       &output)
{
    m_return_val_if_fail(!command.isEmpty(), false);
    return impl->runCommand(command, args, gap_cmd_line, output);
}

void GapProcess::askCompletions()
{
    impl->askCompletions();
}

void GapProcess::die()
{
    impl->die();
}
