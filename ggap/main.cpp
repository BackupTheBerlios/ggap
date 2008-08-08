#include "ggap/app.h"
#include <locale.h>
#include <stdio.h>
#include <QDebug>

#ifdef Q_OS_WIN32

#include <stdlib.h>

static void printMsg(const char *prefix, const char *msg)
{
    FILE *file;
    static bool been_here;

    if (been_here)
        file = fopen("C:\\log.txt", "a");
    else
        file = fopen("C:\\log.txt", "w");

    been_here = true;

    fprintf(file, "%s: %s\n", prefix, msg);
    fclose(file);
}

static void msgHandler(QtMsgType type, const char *msg)
{
    switch (type)
    {
        case QtDebugMsg:
            printMsg("Debug", msg);
            break;
        case QtWarningMsg:
            printMsg("Warning", msg);
            break;
        case QtCriticalMsg:
            printMsg("Critical", msg);
            break;
        case QtFatalMsg:
            printMsg("Fatal", msg);
            abort();
            break;
    }
}

#else

static void msgHandler(QtMsgType type, const char *msg)
{
    switch (type)
    {
        case QtDebugMsg:
            fprintf(stderr, "D: %s\n", msg);
            break;
        case QtWarningMsg:
            fprintf(stderr, "*** Warning: %s\n", msg);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "*** Critical: %s\n", msg);
            break;
        case QtFatalMsg:
            fprintf(stderr, "*** Fatal: %s\n", msg);
            abort();
            break;
    }
}

#endif

#ifdef Q_OS_WIN32

#include <windows.h>
#include <stdlib.h>

static void abicheck()
{
    struct S {
        uint a;
        char b;
        uint c : 1;
    };

    // it is 12 with -mms-bitfields
    if (sizeof(S) != 8)
    {
        MessageBoxA(NULL, "Application appears to be incorrectly built, can not continue",
                    NULL, MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }
}

#else

static void abicheck()
{
}

#endif

int main(int argc, char *argv[])
{
    abicheck();
    setlocale(LC_ALL, "");
    qInstallMsgHandler(msgHandler);
    ggap::App::parseOptions(argc, argv);
    ggap::App app(argc, argv);
    return app.exec();
}
