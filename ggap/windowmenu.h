#ifndef GGAP_WINDOW_MENU_H
#define GGAP_WINDOW_MENU_H

#include <QMenu>
#include <QAction>
#include <QTimer>

namespace ggap {

class AppWindow;

class WindowMenuAction : public QAction {
    Q_OBJECT
    AppWindow *window;

public:
    WindowMenuAction(AppWindow *window, bool active, QObject *parent);
    ~WindowMenuAction();

private Q_SLOTS:
    void updateItem();
    void toggled(bool);
    void triggered();
};

class WindowMenu : public QMenu {
    Q_OBJECT
    AppWindow *window;
    bool update_timer_started;

public:
    WindowMenu(AppWindow *window, QWidget *parent = 0);
    ~WindowMenu();

    static void queueUpdateMenus();

private Q_SLOTS:
    void queueUpdate();
    void updateMenu();
};

}

#endif // GGAP_WINDOW_MENU_H
