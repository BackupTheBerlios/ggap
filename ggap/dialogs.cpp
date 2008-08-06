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
}

NiceDialog::~NiceDialog()
{
}


static QPointer<AboutDialog> about_dialog_instance;

AboutDialog::AboutDialog() :
    NiceDialog(0)
{
    Ui::AboutDialog ui;
    ui.setupUi(this);
    ui.labelVersion->setText("Version " GGAP_VERSION);
    ui.viewLicense->setPlainText(util::getFileText(":/COPYING"));
    ui.viewCredits->setPlainText(util::getFileText(":/THANKS"));
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::showDialog()
{
    if (!about_dialog_instance)
    {
        about_dialog_instance = new AboutDialog;
        about_dialog_instance->show();
    }

    about_dialog_instance->raise();
    about_dialog_instance->activateWindow();
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
                tr("GGAP Worksheets (*.gws);;All files (*)"),
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
#define FILTER_WORKSPACE "GGAP Workspace (*.gws)"

static QString filterString()
{
    if (prefsValue(Prefs::DefaultFileFormat) == "worksheet")
        return tr(FILTER_WORKSHEET) + ";;" + tr(FILTER_WORKSPACE);
    else
        return tr(FILTER_WORKSPACE) + ";;" + tr(FILTER_WORKSHEET);
}

#if 1 && (defined(Q_OS_MAC) || defined(Q_OS_WIN32))

QString ggap::getSaveFileName(WsWindow *window, bool *saveWorkspace)
{
    QString name;

    if (window)
    {
        name = window->doc()->filename();
        if (name.isEmpty())
        {
            name = window->doc()->displayName();
            if (!name.endsWith(".gws"))
                name.append(".gws");
        }
    }

    QString filter;
    QString filename = QFileDialog::getSaveFileName(window, tr("Save As"), name,
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
    QString name;

    if (window)
    {
        name = window->doc()->filename();
        if (name.isEmpty())
        {
            name = window->doc()->displayName();
            if (!name.endsWith(".gws"))
                name.append(".gws");
        }
    }

    QFileDialog dlg(window, tr("Save As"), name, filterString());
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setDefaultSuffix("gws");
    dlg.setResolveSymlinks(false);

    QByteArray state = stateValue(PREFS_SAVE_DIALOG_STATE).toByteArray();
    if (!state.isEmpty())
        dlg.restoreState(state);

    QString filename;

    if (dlg.exec())
        filename = dlg.selectedFiles().value(0);

    if (!filename.isEmpty())
        *saveWorkspace = dlg.selectedFilter() == tr(FILTER_WORKSPACE);

    state = dlg.saveState();
    setStateValue(PREFS_SAVE_DIALOG_STATE, state);

    if (window)
        window->activateWindow();

    return filename;
}

#endif // unix
