#ifndef GGAP_DIALOGS_H
#define GGAP_DIALOGS_H

#include <QMessageBox>

namespace ggap {

class NiceDialog : public QDialog {
protected:
    NiceDialog(QWidget *parent);
    ~NiceDialog();
};

class AboutDialog : public NiceDialog {
    AboutDialog();
    ~AboutDialog();

public:
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
