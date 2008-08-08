#include "config.h"
#include "ggap/app.h"
#include "ggap/dialogs.h"
#include "ggap/wswindow.h"
#include "ggap/worksheet.h"
#include "ggap/prefs.h"
#include "ggap/ui_about.h"
#include "ggap/utils.h"
#include <QtGui>

using namespace ggap;


NiceDialog::NiceDialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);
    new QShortcut(QKeySequence::Close, this, SLOT(close()));
    new QShortcut(QKeySequence("Ctrl+Q"), this, SLOT(quit()));
}

NiceDialog::~NiceDialog()
{
}

void NiceDialog::quit()
{
    gapApp->maybeQuit();
}


static QPointer<AboutDialog> about_dialog_instance;

AboutDialog::AboutDialog() :
    NiceDialog(0)
{
    Ui::AboutDialog ui;
    ui.setupUi(this);
    ui.labelVersion->setText("Version " GGAP_VERSION);
    ui.viewCredits->setPlainText(util::getFileText(":/THANKS"));
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::showDialog()
{
    NiceDialog::showDialog(about_dialog_instance);
}


static QString makePrimaryText(const QString &primaryText)
{
    QString text = primaryText;
    const QString tmpl("This document could not be saved because");
    for (int i = 0; i < tmpl.length() - primaryText.length(); ++i)
        text.append("  ");
    return text;
}

QMessageBox::StandardButton
ggap::informationDialog(QWidget *parent,
                        const QString &title,
                        const QString &text,
                        const QString &secondaryText,
                        QMessageBox::StandardButtons buttons,
                        QMessageBox::StandardButton defaultButton)
{
    QMessageBox box(QMessageBox::Information, title,
                    makePrimaryText(text), buttons, parent);
    box.setDefaultButton(defaultButton);
    box.setInformativeText(secondaryText);
    return QMessageBox::StandardButton(box.exec());
}

QString ggap::getExeFileName(QWidget *parent, const QString &title, const QString &start)
{
    return QFileDialog::getOpenFileName(parent, title, start, QString(), 0,
                                        // FIXME Qt 4.4 ignores this flag on Mac
                                        QFileDialog::DontResolveSymlinks);
}

QString ggap::getDirName(QWidget *parent, const QString &title, const QString &start)
{
    return QFileDialog::getExistingDirectory(parent, title, start,
                                             // FIXME Qt 4.4 ignores this flag on Mac
                                             QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
}

inline static QString tr(const char *s)
{
    return s;
}

template<typename T>
static T get_filenames(WsWindow *window,
                       T (*func) (QWidget*, const QString&, const QString&,
                                  const QString&, QString*, QFileDialog::Options))
{
    QWidget *parent = window;
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks;

#ifdef Q_OS_MAC
    if (window && !window->doc()->isEmpty())
        options |= QFileDialog::DontUseSheet;
#endif

    return func(parent, tr("Open File"), QString(),
                tr("GGAP Files (*.gws *.gwp);;All files (*)"),
                0, options);
}

QStringList ggap::getOpenFileNames(WsWindow *window)
{
    return get_filenames(window, QFileDialog::getOpenFileNames);
}

QString ggap::getOpenFileName(WsWindow *window)
{
    return get_filenames(window, QFileDialog::getOpenFileName);
}

#define FILTER_WORKSHEET "GGAP Worksheet (*.gws)"
#define FILTER_WORKSPACE "GGAP Workspace (*.gwp)"

static QString filterString()
{
    if (prefsValue(Prefs::DefaultFileFormat) == "worksheet")
        return tr(FILTER_WORKSHEET) + ";;" + tr(FILTER_WORKSPACE);
    else
        return tr(FILTER_WORKSPACE) + ";;" + tr(FILTER_WORKSHEET);
}

static QString getNameFromWindow(WsWindow *window)
{
    QString name;

    if (window)
    {
        name = window->doc()->filename();
        if (name.isEmpty())
            name = window->doc()->displayName();
        if (name.endsWith(".gws") || name.endsWith(".gwp"))
            name.chop(4);
    }

    return name;
}

#if 1 && (defined(Q_OS_MAC) || defined(Q_OS_WIN32))

QString ggap::getSaveFileName(WsWindow *window, bool *saveWorkspace)
{
    QString filter;
    QString filename = QFileDialog::getSaveFileName(window, tr("Save As"),
                                                    getNameFromWindow(window),
                                                    filterString(), &filter,
                                                    QFileDialog::DontResolveSymlinks);

    if (!filename.isEmpty())
        *saveWorkspace = filter == tr(FILTER_WORKSPACE);

    return filename;
}

#else // unix

#define PREFS_SAVE_DIALOG_STATE "ui/save-dialog-state"

QString ggap::getSaveFileName(WsWindow *window, bool *saveWorkspace)
{
    QPointer<WsWindow> window_ptr = window;

    QFileDialog dlg(window, tr("Save As"),
                    getNameFromWindow(window),
                    filterString());
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setResolveSymlinks(false);
    dlg.setConfirmOverwrite(false);

    QByteArray state = stateValue(PREFS_SAVE_DIALOG_STATE).toByteArray();
    if (!state.isEmpty())
        dlg.restoreState(state);

    QString filename;
    bool workspace = true;

    while (dlg.exec())
    {
        filename = dlg.selectedFiles().value(0);
        workspace = dlg.selectedFilter() == tr(FILTER_WORKSPACE);

        if (QFileInfo(filename).suffix().isEmpty())
        {
            if (workspace)
                filename += ".gwp";
            else
                filename += ".gws";
        }

        if (QFileInfo(filename).exists())
        {
            QString basename = QFileInfo(filename).fileName();
            QMessageBox::StandardButton btn =
                QMessageBox::warning(&dlg, "",
                                     QString("A file named \"%1\" already exists in this location. "
                                             "Do you want to replace it with the one you are saving?").arg(basename),
                                     QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
            if (btn == QMessageBox::Ok)
                break;
        }

        filename = QString();
    }

    if (!filename.isEmpty())
        *saveWorkspace = workspace;

    state = dlg.saveState();
    setStateValue(PREFS_SAVE_DIALOG_STATE, state);

    if (window_ptr)
        window_ptr->activateWindow();

    return filename;
}

#endif // unix
