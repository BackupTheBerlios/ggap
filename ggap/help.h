#ifndef GGAP_HELP_H
#define GGAP_HELP_H

#include <moo-pimpl.h>
#include <ggap/appwindow.h>
#include <QTextBrowser>
#include <QUrl>

namespace ggap {

class HelpViewPrivate;
class HelpView : public QTextBrowser {
    Q_OBJECT
    M_DECLARE_IMPL(HelpView)

public:
    HelpView(QWidget *parent = 0);
    ~HelpView();

    void setSource(const QUrl &url);
    void home();

    void applyPrefs();
    void setFontFromPrefs();

    virtual QVariant loadResource(int type, const QUrl &name);

private:
    QVariant checkLoadHtml(const QVariant &data, const QUrl &url);
    QVariant checkLoadCss(const QVariant &data, const QUrl &url);

public Q_SLOTS:
    void up();

Q_SIGNALS:
    void upAvailable(bool available);
};

class HelpBrowserPrivate;
class HelpBrowser : public TextEditWindow {
public:
    static void showWindow(const QUrl &url = QUrl());
    static void applyPrefs();

    static QString parseUrl(const QString &str);

private:
    Q_OBJECT
    M_DECLARE_IMPL(HelpBrowser)

    HelpBrowser();
    ~HelpBrowser();

    virtual QTextEdit *textEdit();

    HelpView *view();

    bool eventFilter(QObject *obj, QEvent *event);

    void doApplyPrefs();
    void doFind(const QString &text, bool forward, bool atCursorOk = true);

private Q_SLOTS:
    void startFind();
    void cancelFind();
    void findNext();
    void findPrev();
    void findEntryTextEdited(const QString &text);
    void openAddress(const QString &address);
    void viewSourceChanged(const QUrl &url);
    void on_actionAddBookmark_triggered();
    void on_actionGoNextPage_triggered();
    void on_actionGoPrevPage_triggered();
    void on_actionGo_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionZoomNormal_triggered();
    void on_actionShowContents_triggered();
    void on_actionShowBookmarks_triggered();
    void on_actionShowSearch_triggered();
};

}

#endif // GGAP_HELP_H
