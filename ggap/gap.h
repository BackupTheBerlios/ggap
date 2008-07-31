#ifndef GGAP_GAP_H
#define GGAP_GAP_H

#include <QStringList>
#include <ggap/proc.h>

namespace ggap {

struct GapOptions {
    QString rootDir();
    QString docDir();
    QString exe();
    QString args();
};

GapCommand makeGapCommand(const QString &workspace, const QStringList &flags);

#define GGAP_CMD_RUN_COMMAND "run-command"
QString gapCmdRunCommand(const QString &cmdname, const QString &args = QString());

QString gapCmdHelp(const QString &text);
QString gapCmdSaveWorkspace(const QString &filename);

#define GGAP_DTC_FIXED      'f'
#define GGAP_DTC_VARIABLE   'v'
#define GGAP_DTC_END        'e'

} // namespace ggap

#endif // GGAP_GAP_H
