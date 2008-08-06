#ifndef GGAP_PREFS_DIALOG_H
#define GGAP_PREFS_DIALOG_H

#include <ggap/dialogs.h>
#include <QPointer>

namespace ggap {

class PrefsDialogBase : public NiceDialog {

    void accept();
    virtual void apply() = 0;

protected:
    PrefsDialogBase(QWidget *parent);
    virtual void init() = 0;
};

class PrefsDialog : public PrefsDialogBase {
    Q_OBJECT
    struct Private;
    Private *priv;

    void init();
    void apply();

public:
    PrefsDialog();
    ~PrefsDialog();

    static void showDialog();
    static void execDialog();

private Q_SLOTS:
    void on_buttonGapDir_clicked();
    void on_buttonGapExe_clicked();
    void on_buttonWorksheetFont_clicked();
    void on_buttonHelpFont_clicked();
    void otherPrefs();
};

}

#endif // GGAP_PREFS_DIALOG_H
