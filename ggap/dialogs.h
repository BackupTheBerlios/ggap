#ifndef GGAP_DIALOGS_H
#define GGAP_DIALOGS_H

#include <QMessageBox>
#include <QPointer>

namespace ggap {

class NiceDialog : public QDialog {
    template<typename T>
    static T *createDialog()
    {
        T *dlg = new T;
        dlg->show();
        dlg->raise();
        dlg->activateWindow();
        return dlg;
    }

protected:
    NiceDialog(QWidget *parent);
    ~NiceDialog();

    template<typename T>
    static void showDialog(QPointer<T> &dlg)
    {
        if (!dlg)
            dlg = createDialog<T>();
    }
};

struct AboutDialog : public NiceDialog {
    AboutDialog();
    ~AboutDialog();
    static void showDialog();
};

class WsWindow;

QStringList getOpenFileNames(WsWindow *window);
QString getOpenFileName(WsWindow *window);
QString getSaveFileName(WsWindow *window, bool *saveWorkspace);
QString getExeFileName(QWidget *parent, const QString &title, const QString &start);
QString getDirName(QWidget *parent, const QString &title, const QString &start);

QMessageBox::StandardButton informationDialog(QWidget *parent,
                                              const QString &title,
                                              const QString &text,
                                              const QString &secondaryText = QString(),
                                              QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                              QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

}

#endif // GGAP_DIALOGS_H
