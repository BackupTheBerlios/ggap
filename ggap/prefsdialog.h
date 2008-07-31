#ifndef GGAP_PREFS_DIALOG_H
#define GGAP_PREFS_DIALOG_H

#include <ggap/dialogs.h>

namespace ggap {

class PrefsDialog : public NiceDialog {
    Q_OBJECT
    struct Private;
    Private *priv;

    void init();
    void apply();
    void accept();

    PrefsDialog();
    ~PrefsDialog();

public:
    static void showDialog();
    static void execDialog();

private Q_SLOTS:
    void on_buttonGapDir_clicked();
    void on_buttonGapExe_clicked();
    void on_buttonWorksheetFont_clicked();
    void on_buttonHelpFont_clicked();
};

}

#endif // GGAP_PREFS_DIALOG_H
