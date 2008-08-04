#ifndef GGAP_APP_WINDOW_H
#define GGAP_APP_WINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <ggap/windowmenu.h>

class QTextEdit;
class QPrinter;

namespace ggap {

class WindowMenu;
class AppWindow : public QMainWindow {
    Q_OBJECT

    WindowMenu *window_menu;

public:
    static QList<AppWindow*> listWindows();
    WindowMenu *windowMenu();

protected:
    AppWindow(const QString &name);
    ~AppWindow();

    virtual void loadUiConfig();
    virtual void saveUiConfig();

    void closeEvent(QCloseEvent *event);

    void setActionShortcut(const char *name, const QKeySequence &shortcut);

    void connectAction(const char *action, const char *slot);
    void connectAction(QAction *action, const char *slot);
    void connectAction(const char *action, QObject *receiver, const char *slot);
    void connectAction(QAction *action, QObject *receiver, const char *slot);

protected Q_SLOTS:
    void setWindowFilePath(const QString &path);
    void setWindowTitle(const QString &title);
    void setWindowModified(bool modified);

    void zoomWindow();
    void bringAllToFront();

Q_SIGNALS:
    // FIXME Qt should have these
    void windowFilePathChanged(const QString &newPath);
    void windowTitleChanged(const QString &newTitle);
    void windowModifiedChanged(bool modified);
};


class TextEditWindow : public AppWindow {
    Q_OBJECT

protected:
    TextEditWindow(const QString &name);
    ~TextEditWindow();

    virtual void loadUiConfig();

protected Q_SLOTS:
    void printDocument();
    void previewPaintRequested(QPrinter *printer);
    void printPreview();
    void pageSetup();

private:
    virtual QTextEdit *textEdit() = 0;
};

}

#endif // GGAP_APP_WINDOW_H
