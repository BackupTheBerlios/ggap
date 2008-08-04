#include "ggap/wswindow-p.h"
#include "ggap/worksheet.h"
#include "ggap/dialogs.h"
#include "ggap/app.h"
#include "ggap/ui_wswindow.h"
#include "moo-macros.h"
#include <QtGui>

using namespace ggap;

QList<WsWindow*> WsWindowPrivate::windows;

Worksheet *WsWindow::ws()
{
    return ui->worksheet;
}

void WsWindow::init()
{
    ui = new Ui::WsWindow;
    ui->setupUi(this);

    QObject::connect(ui->worksheet, SIGNAL(copyAvailable(bool)), ui->actionCopy, SLOT(setEnabled(bool)));
    QObject::connect(ui->worksheet, SIGNAL(copyAvailable(bool)), ui->actionCut, SLOT(setEnabled(bool)));
    QObject::connect(ui->worksheet, SIGNAL(copyAvailable(bool)), ui->actionDelete, SLOT(setEnabled(bool)));
    QObject::connect(ui->worksheet, SIGNAL(undoAvailable(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
    QObject::connect(ui->worksheet, SIGNAL(redoAvailable(bool)), ui->actionRedo, SLOT(setEnabled(bool)));

    connectAction(ui->actionUndo, ui->worksheet, SLOT(undo()));
    connectAction(ui->actionRedo, ui->worksheet, SLOT(redo()));
    connectAction(ui->actionCut, ui->worksheet, SLOT(cut()));
    connectAction(ui->actionCopy, ui->worksheet, SLOT(copy()));
    connectAction(ui->actionPaste, ui->worksheet, SLOT(paste()));
    connectAction(ui->actionDelete, ui->worksheet, SLOT(deleteSelected()));
    connectAction(ui->actionDeleteBlock, ui->worksheet, SLOT(deleteBlock()));
    connectAction(ui->actionInsertPromptAfter, ui->worksheet, SLOT(addPromptBlockAfterCursor()));
    connectAction(ui->actionInsertPromptBefore, ui->worksheet, SLOT(addPromptBlockBeforeCursor()));
    connectAction(ui->actionInsertTextAfter, ui->worksheet, SLOT(addTextBlockAfterCursor()));
    connectAction(ui->actionInsertTextBefore, ui->worksheet, SLOT(addTextBlockBeforeCursor()));
    connectAction(ui->actionSelectAll, ui->worksheet, SLOT(selectAll()));
    connectAction(ui->actionInterruptGap, ui->worksheet, SLOT(interruptGap()));
    connectAction(ui->actionRestartGap, ui->worksheet, SLOT(restartGap()));

    connectAction(ui->actionNew, SLOT(actionNew()));
    connectAction(ui->actionOpen, SLOT(actionOpen()));
    connectAction(ui->actionSave, SLOT(actionSave()));
    connectAction(ui->actionSaveAs, SLOT(actionSaveAs()));
    connectAction(ui->actionHelp, gapApp, SLOT(gapHelp()));

    connect(ui->worksheet, SIGNAL(docStateChanged()), SLOT(docStateChanged()));
    connect(ui->worksheet, SIGNAL(savingFinished(bool,QString)), SLOT(savingFinished(bool,QString)));
    connect(ui->worksheet, SIGNAL(modificationChanged(bool)), SLOT(setWindowModified(bool)));
    connect(ui->worksheet, SIGNAL(filenameChanged(QString)), SLOT(setWindowFilePath(QString)));
    connect(ui->worksheet, SIGNAL(gapStateChanged()), SLOT(gapStateChanged()));

    gapStateChanged();

    impl->setupFormatToolbar(ui);
    impl->setupMenuBar(ui);

#ifdef Q_OS_MAC
    setActionShortcut("actionInterruptGap", QKeySequence("Meta+C"));
#else
    setActionShortcut("actionInterruptGap", QKeySequence("Ctrl+Pause"));
#endif

    setActionShortcut("actionHelp", QKeySequence::HelpContents);
    loadUiConfig();
}

QTextEdit *WsWindow::textEdit()
{
    return ws();
}

void WsWindow::gapStateChanged()
{
    switch (ws()->gapState())
    {
        case GapProcess::Dead:
            ui->statusbar->showMessage("GAP not running");
            break;
        case GapProcess::Starting:
            ui->statusbar->showMessage("Starting");
            break;
        case GapProcess::Loading:
            ui->statusbar->showMessage("Loading");
            break;
        case GapProcess::Busy:
            ui->statusbar->showMessage("Busy");
            break;
        case GapProcess::InPrompt:
            ui->statusbar->clearMessage();
            break;
    }
}

void WsWindow::docStateChanged()
{
    switch (ws()->docState())
    {
        case Worksheet::Saving:
            ui->statusbar->showMessage("Saving");
            break;
        case Worksheet::Idle:
            ui->statusbar->clearMessage();
            break;
    }
}

bool WsWindow::loadFile(const QString &filename)
{
    QString error;
    bool result = ws()->load(filename, error);
    if (!result)
        QMessageBox::critical(this, "", QString("Could not open file\n") + error);
    else
        RecentMenu::addFile(ws()->filename());
    return result;
}

void WsWindow::ensureWindow()
{
    if (WsWindowPrivate::windows.isEmpty())
    {
        WsWindow *w = new WsWindow;
        w->present(0);
    }
}

void WsWindow::savingFinished(bool success, const QString &error)
{
    bool close_on_save = impl->close_on_save;
    impl->close_on_save = false;

    if (success)
        RecentMenu::addFile(ws()->filename());

    if (!success)
        QMessageBox::critical(this, "", QString("Could not save file\n") + error);
    else if (close_on_save)
        close(); // XXX this has no effect
}

void WsWindow::saveFile(const QString &name, bool close_on_save, bool ask_filename)
{
    m_return_if_fail(ws()->docState() == Worksheet::Idle);

    QString filename = name;
    File::Type type = File::Worksheet;
    bool type_set = false;
    if (filename.isEmpty())
        filename = ws()->filename();
    if (ask_filename || filename.isEmpty())
    {
        bool saveWorkspace = false;
        filename = getSaveFileName(this, &saveWorkspace);
        if (saveWorkspace)
            type = File::Workspace;
        type_set = true;
    }
    if (filename.isEmpty())
        return;

    impl->close_on_save = close_on_save;
    if (!type_set)
        ws()->save(filename);
    else
        ws()->save(filename, type);
    // returns after saving
}

WsWindow::WsWindow() :
    TextEditWindow("worksheet"),
    impl(this), ui(0)
{
    init();
}

WsWindow::~WsWindow()
{
    delete ui;
}

void WsWindow::present(WsWindow *old)
{
    if (old)
        move(old->x() + 22, old->y() + 22); // XXX

    if (ws()->filename().isEmpty())
        setWindowFilePath(ws()->displayBasename());

    if (!ws()->isRunning())
        ws()->start();

    // XXX
    ws()->emitCursorPositionChanged();

    show();
}


void WsWindowPrivate::applyPrefs()
{
    foreach (WsWindow *w, windows)
        w->doc()->applyPrefs();
}


void WsWindow::actionNew()
{
    WsWindow *win = new WsWindow;
    win->present(this);
}

void WsWindow::loadFile(const QString &filename, WsWindow *active)
{
    m_return_if_fail(!filename.isEmpty());

    if (WsWindow *old = WsWindowPrivate::find(filename))
    {
        RecentMenu::addFile(old->ws()->filename());
        old->raise();
        old->activateWindow();
        return;
    }

    if (active && active->ws()->isEmpty())
    {
        active->loadFile(filename);
    }
    else
    {
        WsWindow *win = new WsWindow;

        if (!win->loadFile(filename))
        {
            delete win;
            return;
        }

        win->present(active);
    }
}

void WsWindow::openFile(const QString &filename, WsWindow *window)
{
    // XXX
    if (!window)
        window = dynamic_cast<WsWindow*>(QApplication::activeWindow());
    if (!window)
    {
        QWidgetList windows = QApplication::topLevelWidgets();
        foreach (QWidget *w, windows)
            if ((window = dynamic_cast<WsWindow*>(w)))
                break;
    }

    loadFile(filename, window);
}

void WsWindow::actionOpen()
{
    QString filename = getOpenFileName(this);
    if (!filename.isEmpty())
        loadFile(filename, this);
}

void WsWindow::actionSave()
{
    saveFile("", false);
}

void WsWindow::actionSaveAs()
{
    saveFile("", false, true);
}


static void demandAttention(WsWindow *window)
{
//     if (!qApp->activeWindow())
//     {
//         window->raise();
//         qApp->alert(window);
//     }
//     else
//     {
        window->raise();
        window->activateWindow();
//     }
}

void WsWindow::closeEvent(QCloseEvent *e)
{
    if (ws()->docState() != Worksheet::Idle)
    {
        demandAttention(this);
//         raise();
//         activateWindow();
        QApplication::beep();
        e->ignore();
        return;
    }

    if (ws()->isModified())
    {
        demandAttention(this);
//         raise();
//         activateWindow();

        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.setText(tr("Do you want to save changes to this document before closing?"));
        msgBox.setInformativeText(tr("If you don't save, your changes will be lost."));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowModality(Qt::WindowModal);

        switch (msgBox.exec())
        {
            case QMessageBox::Cancel:
                e->ignore();
                return;
            case QMessageBox::Save:
                saveFile("", true);
                e->ignore();
                return;
        }
    }

    AppWindow::closeEvent(e);
}

void WsWindowPrivate::setupMenuBar(Ui::WsWindow *ui)
{
    M_Q(WsWindow);
    ui->menubar->insertMenu(ui->menuHelp->menuAction(), q->windowMenu());

    QMenu *recent_menu = new RecentMenu(pub, ui->menuFile);
    ui->menuFile->insertMenu(ui->actionSave, recent_menu);

    ui->menuView->addAction(ui->mainToolbar->toggleViewAction());
    QAction *a = formatToolbar->toggleViewAction();
    ui->menuView->addAction(a);
    ui->worksheet->connect(a, SIGNAL(toggled(bool)), SLOT(setTrackCurrentFormat(bool)));
}

static void addActions(QToolBar *toolbar, QActionGroup *group)
{
    QList<QAction*> actions = group->actions();
    foreach (QAction *a, actions)
        toolbar->addAction(a);
}

void WsWindowPrivate::setupFormatToolbar(Ui::WsWindow *ui)
{
    formatToolbar = new QToolBar(pub);
    formatToolbar->hide();
    formatToolbar->setObjectName(QString::fromUtf8("formatToolbar"));
    formatToolbar->setGeometry(QRect(0, 56, 606, 13));
    pub->addToolBar(Qt::TopToolBarArea, formatToolbar);
    pub->insertToolBarBreak(formatToolbar);
    formatToolbar->setWindowTitle(tr("Formatting Toolbar"));

    QToolBar *t = formatToolbar;
    t->addAction(ui->worksheet->actionFontButton(t));
    t->addAction(ui->worksheet->actionBold(t));
    t->addAction(ui->worksheet->actionItalic(t));
    t->addAction(ui->worksheet->actionUnderline(t));
    t->addSeparator();
    addActions(t, ui->worksheet->actionGroupJustify(t));
    t->addSeparator();
    addActions(t, ui->worksheet->actionGroupSupSubScript(t));
    t->addSeparator();
    t->addAction(ui->worksheet->actionTextColor(t));
}


/**************************************************************************/
/* RecentMenu
 */

#define MAX_RECENT_FILES 10
#define PREFS_RECENT_FILES "files/recent-files"

struct ggap::RecentMenuPrivate {
    QList<QAction*> actions;
    bool update_timer_started;
    WsWindow *window;
    static QList<RecentMenu*> menus;

    RecentMenuPrivate(RecentMenu *pub, WsWindow *window);
    ~RecentMenuPrivate();
};

QList<RecentMenu*> RecentMenuPrivate::menus;

RecentMenuPrivate::RecentMenuPrivate(RecentMenu *menu, WsWindow *window) :
    update_timer_started(false), window(window)
{
    for (int i = 0; i < MAX_RECENT_FILES; ++i)
    {
        QAction *a = new QAction(menu);
        QObject::connect(a, SIGNAL(triggered()), menu, SLOT(actionTriggered()));
        actions.append(a);
    }
}

RecentMenuPrivate::~RecentMenuPrivate()
{
}

RecentMenu::RecentMenu(WsWindow *window, QWidget *parent) :
    QMenu(parent), impl(new RecentMenuPrivate(this, window))
{
    setTitle(tr("Open &Recent"));

    foreach (QAction *a, impl->actions)
        addAction(a);

    addSeparator();
    QAction *clear_menu = new QAction(tr("Clear Menu"), this);
    connect(clear_menu, SIGNAL(triggered()), SLOT(clearMenu()));
    addAction(clear_menu);

    RecentMenuPrivate::menus.append(this);
    updateMenu();
}

RecentMenu::~RecentMenu()
{
    RecentMenuPrivate::menus.removeAll(this);
    delete impl;
}

void RecentMenu::queueUpdate()
{
    if (!impl->update_timer_started)
    {
        impl->update_timer_started = true;
        QTimer::singleShot(0, this, SLOT(updateMenu()));
    }
}

void RecentMenu::updateMenu()
{
    impl->update_timer_started = false;

    QStringList files = stateValue(PREFS_RECENT_FILES).toStringList();
    int n = qMin(files.size(), MAX_RECENT_FILES);

    for (int i = 0; i < n; ++i)
    {
        QString text = tr("&%1. %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        impl->actions.at(i)->setText(text);
        impl->actions.at(i)->setData(files[i]);
        impl->actions.at(i)->setVisible(true);
    }

    for (int i = n; i < MAX_RECENT_FILES; ++i)
        impl->actions.at(i)->setVisible(false);

     setEnabled(n > 0);
}

void RecentMenu::actionTriggered()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    if (action)
    {
        QString filename = action->data().toString();
        if (!filename.isEmpty())
            WsWindow::openFile(filename, impl->window);
    }
}

void RecentMenu::clearMenu()
{
    clearList();
}

void RecentMenu::queueUpdateAll()
{
    foreach (RecentMenu *m, RecentMenuPrivate::menus)
        m->queueUpdate();
}

void RecentMenu::addFile(const QString &path)
{
    QStringList files = stateValue(PREFS_RECENT_FILES).toStringList();
    files.removeAll(path);
    files.prepend(path);
    while (files.size() > MAX_RECENT_FILES)
        files.removeLast();
    setStateValue(PREFS_RECENT_FILES, files);
    queueUpdateAll();
}

void RecentMenu::clearList()
{
    unsetStateValue(PREFS_RECENT_FILES);
    queueUpdateAll();
}
