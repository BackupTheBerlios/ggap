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
            fprintf(stderr, "W: %s\n", msg);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "C: %s\n", msg);
            break;
        case QtFatalMsg:
            fprintf(stderr, "F: %s\n", msg);
            abort();
            break;
    }
}

#endif

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    qInstallMsgHandler(msgHandler);
    ggap::App::parseOptions(argc, argv);
    ggap::App app(argc, argv);
    return app.exec();
}
