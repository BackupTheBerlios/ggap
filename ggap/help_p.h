#ifndef GGAP_HELP_P_H
#define GGAP_HELP_P_H

#include "ggap/help.h"
#include "ggap/ui_help.h"
#include <QComboBox>
#include <QPointer>

namespace ggap {

class HelpViewPrivate {
    M_DECLARE_PUBLIC(HelpView)

    HelpViewPrivate(HelpView *p);
};

class HelpBrowserPrivate {
    M_DECLARE_PUBLIC(HelpBrowser)

    static QPointer<HelpBrowser> instance;

    Ui::HelpBrowser ui;
    QComboBox *addressEntry;

    HelpBrowserPrivate(HelpBrowser *p);

    void setupUi();
};

}

#endif // GGAP_HELP_P_H
