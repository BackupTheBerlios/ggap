#ifndef GAP_WORKSHEET_H
#define GAP_WORKSHEET_H

#include <moows/worksheet.h>
#include <ggap/proc.h>

namespace ggap {

namespace File {
enum Type {
    Worksheet,
    Workspace
};
}

class WorksheetScriptProxy;
class WorksheetPrivate;
class Worksheet : public moo::ws::Worksheet {
    Q_OBJECT
    M_DECLARE_IMPL(Worksheet)

    friend class WorksheetScriptProxy;

    Q_PROPERTY(QString filename READ filename)
    Q_PROPERTY(QString displayName READ displayName)
    Q_PROPERTY(QString displayBasename READ displayBasename)
    Q_PROPERTY(bool modified READ isModified WRITE setModified)
    Q_PROPERTY(bool empty READ isEmpty)

public:
    Worksheet(QWidget *parent = 0);
    ~Worksheet();

    enum DocState {
        Idle,
        Saving
    };

    DocState docState() const;
    bool isUntitled() const;
    QString filename() const;
    QString displayName() const;
    QString displayBasename() const;
    bool isModified() const;
    bool isEmpty() const;
    void setModified(bool modified);

    void start();
    bool isRunning();
    bool load(const QString &filename, QString &error);
    void save(const QString &filename);
    void save(const QString &filename, File::Type type);

    GapProcess::State gapState() const;

public Q_SLOTS:
    void interruptGap();
    void restartGap();
    void applyPrefs();

private:
    void setFilename(const QString &filename);
    void setDocState(DocState state);
    void processInput(const QStringList &lines);

protected:
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);

private Q_SLOTS:
    void resize_idle();
    void gapScript(const QString &text);
    void gapMarkup(const QString &text);
    void gapOutput(const QString &text, GapProcess::OutputType type);
    void gapPrompt(const QString &text, bool first_time);
    void gapGlobalsChanged(const char *data, uint len, GapProcess::GlobalsChange);

    void gapStarted(GapProcess::StartResult result, const QString &output);
    void gapError(int exitCode, QProcess::ExitStatus exitStatus, const QString &output);
    void gapDied(int exitCode, QProcess::ExitStatus exitStatus);

Q_SIGNALS:
    void docStateChanged();
    void filenameChanged(const QString &newFileName);
    void modificationChanged(bool);
    void savingFinished(bool success, const QString &msg);

    void gapStateChanged();
    void gapExited();
};

} // namespace ggap

#endif // GAP_WORKSHEET_H
