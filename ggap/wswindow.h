#ifndef GAP_WS_WINDOW_H
#define GAP_WS_WINDOW_H

#include <moo-pimpl.h>
#include <ggap/appwindow.h>
#include <QDialog>
#include <QPrinter>

namespace Ui {
class WsWindow;
}

namespace ggap {

class Worksheet;
class WsWindowPrivate;

class WsWindow : public TextEditWindow {
    Q_OBJECT
    M_DECLARE_IMPL(WsWindow)

    Ui::WsWindow *ui;
    Worksheet *ws();

public:
    WsWindow();

    static void ensureWindow();
    static void openFile(const QString &filename, WsWindow *window = 0);
    void present(WsWindow *old);

protected:
    ~WsWindow();

public:
    Worksheet *doc() { return ws(); }

private:
    virtual QTextEdit *textEdit();

    void init();
    void update_window_list();

    bool loadFile(const QString &filename);
    void saveFile(const QString &filename, bool close_on_save, bool ask_filename = false);

    static void loadFile(const QString &filename, WsWindow *active);

    void closeEvent(QCloseEvent *e);

private Q_SLOTS:
    void gapStateChanged();
    void docStateChanged();
    void savingFinished(bool, const QString&);

    void actionNew();
    void actionOpen();
    void actionSave();
    void actionSaveAs();
};

} // namespace ggap

#endif // GAP_WS_WINDOW_H
