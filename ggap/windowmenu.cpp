#include "ggap/windowmenu.h"
#include "ggap/appwindow.h"
#include <QFileInfo>

using namespace ggap;

WindowMenuAction::WindowMenuAction(AppWindow *window, bool active, QObject *parent) :
    QAction(parent), window(window)
{
    connect(window, SIGNAL(windowFilePathChanged(const QString&)), SLOT(updateItem()));
    connect(window, SIGNAL(windowTitleChanged(const QString&)), SLOT(updateItem()));
    connect(window, SIGNAL(windowModifiedChanged(bool)), SLOT(updateItem()));

    updateItem();

    if (active)
    {
        setCheckable(true);
        setChecked(true);
        connect(this, SIGNAL(toggled(bool)), SLOT(toggled(bool)));
    }
    else
    {
        connect(this, SIGNAL(triggered()), SLOT(triggered()));
    }
}

WindowMenuAction::~WindowMenuAction()
{
}

void WindowMenuAction::toggled(bool checked)
{
    if (!checked)
        setChecked(true);
}

void WindowMenuAction::triggered()
{
    window->raise();
    window->activateWindow();
}

static QString get_fullname(AppWindow *window)
{
#if QT_VERSION >= 0x040400
    QString name = window->windowFilePath();
    if (name.isEmpty())
        name = window->windowTitle();
#else
    QString name = window->windowTitle();
#endif
    return name;
}

static QString get_basename(AppWindow *window)
{
#if QT_VERSION >= 0x040400
    QString name = window->windowFilePath();
#else
    QString name;
#endif
    if (name.isEmpty())
        return window->windowTitle();
    else
        return QFileInfo(name).fileName();
}

void WindowMenuAction::updateItem()
{
    QString name = get_fullname(window);
    QString basename = get_basename(window);

    if (window->isWindowModified())
        setText(QString::fromUtf8("\xe2\x97\x8f ") + basename);
    else
        setText(basename);

    if (basename != name)
        setToolTip(name);
}


WindowMenu::WindowMenu(AppWindow *window, QWidget *parent) :
    QMenu(parent), window(window), update_timer_started(false)
{
    setTitle(tr("&Window"));

    addAction("Minimize", window, SLOT(showMinimized()), QKeySequence("Ctrl+M"));
    addAction("Zoom", window, SLOT(zoomWindow()));
    addSeparator();
    addAction("Bring All to Front", window, SLOT(bringAllToFront()));
    addSeparator();

    queueUpdate();
}

WindowMenu::~WindowMenu()
{
}

void WindowMenu::queueUpdateMenus()
{
    QList<AppWindow*> windows = AppWindow::listWindows();
    foreach (AppWindow *w, windows)
        if (WindowMenu *m = w->windowMenu())
            m->queueUpdate();
}

void WindowMenu::queueUpdate()
{
    if (!update_timer_started)
    {
        update_timer_started = true;
        QTimer::singleShot(0, this, SLOT(updateMenu()));
    }
}

static bool cmp_windows(AppWindow *w1, AppWindow *w2)
{
    QString n1 = get_basename(w1);
    QString n2 = get_basename(w2);
    if (n1 < n2)
        return true;
    if (n1 > n2)
        return false;
    n1 = get_fullname(w1);
    n2 = get_fullname(w2);
    return n1 < n2;
}

void WindowMenu::updateMenu()
{
    update_timer_started = false;

    QList<QAction*> old_actions = actions();
    foreach (QAction *a, old_actions)
        if (dynamic_cast<WindowMenuAction*>(a))
            removeAction(a);

    QList<AppWindow*> windows = AppWindow::listWindows();
    qSort(windows.begin(), windows.end(), cmp_windows);

    foreach (AppWindow *w, windows)
    {
        QAction *a = new WindowMenuAction(w, w == window, this);
        addAction(a);
    }
}
