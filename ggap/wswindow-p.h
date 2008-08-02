#ifndef GAP_WS_WINDOW_P_H
#define GAP_WS_WINDOW_P_H

#include "ggap/wswindow.h"
#include "ggap/worksheet.h"
#include "ggap/utils.h"
#include <QDialog>
#include <QPointer>
#include <QFile>
#include <QFileInfo>
#include <QMenuBar>
#include <QCoreApplication>

namespace ggap {

class RecentMenuPrivate;
class RecentMenu : public QMenu {
    Q_OBJECT
    friend class RecentMenuPrivate;
    RecentMenuPrivate *impl;

public:
    RecentMenu(WsWindow *window, QWidget *parent = 0);
    ~RecentMenu();

    static void addFile(const QString &path);
private:
    static void clearList();
    static void queueUpdateAll();

private Q_SLOTS:
    void queueUpdate();
    void updateMenu();
    void clearMenu();
    void actionTriggered();
};

struct WsWindowPrivate {
    M_DECLARE_PUBLIC(WsWindow)

    bool close_on_save;
    RecentMenu *recent_menu;
    QToolBar *formatToolbar;

    static QList<WsWindow*> windows;

    WsWindowPrivate(WsWindow *q) :
        pub(q),
        close_on_save(false),
        recent_menu(0),
        formatToolbar(0)
    {
        windows.append(q);
    }

    ~WsWindowPrivate()
    {
        M_Q(WsWindow);
        windows.removeAll(q);
    }

    static WsWindow *find(const QString &filename)
    {
        QString absname = QFileInfo(filename).absoluteFilePath();
        foreach (WsWindow *w, windows)
            if (w->ws()->filename() == absname)
                return w;
        return 0;
    }

    void setupMenuBar(Ui::WsWindow *ui);
    void setupFormatToolbar(Ui::WsWindow *ui);

    Q_DECLARE_TR_FUNCTIONS(WsWindow)

public:
    static void applyPrefs();
};

} // namespace ggap

#endif // GAP_WS_WINDOW_P_H
