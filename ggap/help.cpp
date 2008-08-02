#include "ggap/help_p.h"
#include "ggap/dialogs.h"
#include "ggap/gap.h"
#include "ggap/prefs.h"
#include "ggap/prefsdialog.h"
#include <QtGui>

using namespace ggap;

QPointer<HelpBrowser> HelpBrowserPrivate::instance;

HelpView::HelpView(QWidget *parent) :
    QTextBrowser(parent),
    impl(this)
{
    applyPrefs();
}

HelpView::~HelpView()
{
}

HelpViewPrivate::HelpViewPrivate(HelpView *p) :
    pub(p)
{
}

void HelpView::setFontFromPrefs()
{
    setFont(prefsValue(Prefs::HelpFont));
}

void HelpView::applyPrefs()
{
    setFontFromPrefs();
}


QString HelpBrowser::parseUrl(const QString &str)
{
    if (!str.startsWith("gfile:"))
        return str;

    QString path = str.mid(6);
    QUrl url = QUrl::fromLocalFile(path);
    url.setScheme("gfile");
    return url.toString();
}

void HelpView::setSource(const QUrl &url)
{
    QUrl realUrl = url;
    if (realUrl.scheme() == "gfile")
        realUrl.setScheme("file");

    QTextBrowser::setSource(realUrl);
    QString p = source().toLocalFile();
    emit upAvailable(!p.isEmpty() && !QFileInfo(p).isRoot());
}

void HelpView::up()
{
    QUrl url = source();
    QString path = url.toLocalFile();
    if (!path.isEmpty())
        setSource(QUrl::fromLocalFile(QFileInfo(path).path()));
}

void HelpView::home()
{
    QString path = prefsValue(Prefs::HelpHomePage);
    if (path.isEmpty() || !QFileInfo(path).exists())
        path = GapOptions().docDir() + "/htm/ref/chapters.htm";
    if (QFileInfo(path).exists())
        setSource(QUrl::fromLocalFile(path));
    else
        setSource(QUrl());
}

static QVariant loadDir(const QFileInfo &fi)
{
    QString str;
    QTextStream s(&str, QIODevice::WriteOnly);

    s << "<HTML>\n";
    s << " <HEAD>\n";
    s << "  <TITLE>" << Qt::escape(QDir::toNativeSeparators(fi.filePath())) << "</TITLE>\n";
    s << " </HEAD>\n";
    s << " <BODY>\n";
    s << "<H1>" << Qt::escape(QDir::toNativeSeparators(fi.filePath())) << "</H1>\n";
    s << "<HR><PRE>\n";

    if (!fi.isRoot())
        s << "<IMG SRC=\":/icons16/go-up.png\" ALT=\"[UP]\"> <A HREF=\""
          << QUrl::fromLocalFile(fi.path()).toString() << "\">..</A>\n";

    QString dirsep = QDir::toNativeSeparators("/");
    QDir d(fi.filePath());
    const QFileInfoList list = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot,
                                               QDir::Name | QDir::IgnoreCase);
    foreach (const QFileInfo &entry, list)
        if (entry.isDir())
            s << "<IMG SRC=\":/icons16/folder.png\" ALT=\"[DIR]\"> <A HREF=\""
              << QUrl::fromLocalFile(entry.filePath()).toString()
              << "\">" << Qt::escape(entry.fileName()) << dirsep << "</A>\n";
        else
            s << "<IMG SRC=\":/icons16/file.png\" ALT=\"[FILE]\"> <A HREF=\""
              << QUrl::fromLocalFile(entry.filePath()).toString()
              << "\">" << Qt::escape(entry.fileName()) << "</A>\n";

    s << "</PRE><HR>\n";
    s << "</BODY></HTML>\n";

    return str;
}

static QString htmlFromPlainText(const QString &text)
{
    QString html = "<html><body><pre>";
    html.append(Qt::escape(text));
    html.append("</pre></body></html>");
    return html;
    return Qt::convertFromPlainText(text);
}

QVariant HelpView::checkLoadHtml(const QVariant &data, const QUrl &url)
{
    if (data.isNull())
        return QString("<html><body><h1>Not found</h1></body></html>");

    if (data.type() == QVariant::String)
    {
        const QString s = data.toString();
        if (s.contains("<html", Qt::CaseInsensitive)) // XXX
            return data;
        else
            return htmlFromPlainText(s);
    }
    else if (data.type() == QVariant::ByteArray)
    {
        const QByteArray a = data.toByteArray();
        if (a.contains("<html") || a.contains("<HTML")) // XXX
            return data;
        else
            return htmlFromPlainText(QString(a)); // XXX
    }
    else
    {
        return data;
    }
}

QVariant HelpView::checkLoadCss(const QVariant &data, const QUrl &url)
{
    if (!data.isNull())
        return data;

    QString filename = url.toLocalFile();
    QRegExp re("(.*/pkg/[^/]+/doc/)manual.css");
    re.setCaseSensitivity(Qt::CaseInsensitive);

    if (re.exactMatch(filename))
    {
        QString newFilename = re.cap(1) + "gapdoc.css";
        if (QFileInfo(newFilename).exists())
        {
            qDebug() << filename << "not found, trying" << newFilename;
            return loadResource(QTextDocument::StyleSheetResource,
                                QUrl::fromLocalFile(newFilename));
        }
    }

    return data;
}

QVariant HelpView::loadResource(int type, const QUrl &url)
{
    if (!url.isRelative())
    {
        QFileInfo fi(url.toLocalFile());
        if (fi.isDir())
            return loadDir(fi);
    }

    const QVariant data = QTextBrowser::loadResource(type, url);

    switch (type)
    {
        case QTextDocument::StyleSheetResource:
            return checkLoadCss(data, url);
        case QTextDocument::HtmlResource:
            return checkLoadHtml(data, url);
        default:
            return data;
    }
}


static void show_implement_me(QWidget *parent, const char *func)
{
    QMessageBox::warning(parent, QString(), QString("IMPLEMENT ME\n%1").arg(func));
}

#define implement_me() show_implement_me(this, __func__)

QTextEdit *HelpBrowser::textEdit()
{
    return view();
}

HelpBrowser::HelpBrowser() :
    TextEditWindow("help"),
    impl(this)
{
    impl->setupUi();
}

HelpBrowser::~HelpBrowser()
{
}

void HelpBrowser::showWindow(const QUrl &url)
{
    if (!HelpBrowserPrivate::instance)
    {
        HelpBrowserPrivate::instance = new HelpBrowser;
        HelpBrowserPrivate::instance->show();
    }

    HelpBrowser *b = HelpBrowserPrivate::instance;
    HelpView *v = b->view();
    b->raise();
    b->activateWindow();

    if (v->source().isEmpty() && !url.isValid())
        v->home();
    else if (!url.isEmpty())
        v->setSource(url);
}

HelpBrowserPrivate::HelpBrowserPrivate(HelpBrowser *p) :
    pub(p), addressEntry(0)
{
}

void HelpBrowserPrivate::setupUi()
{
    M_Q(HelpBrowser);

    ui.setupUi(q);

    QMenu *bookmarkMenu = createBookmarkMenu(ui.actionAddBookmark, ui.menubar);
    ui.menubar->insertMenu(ui.menuHelp->menuAction(), bookmarkMenu);
    QObject::connect(bookmarkMenu, SIGNAL(itemActivated(int)), ui.bookmarkList, SLOT(activateBookmark(int)));

    QObject::connect(ui.helpView, SIGNAL(highlighted(QString)), ui.statusbar, SLOT(showMessage(QString)));
    QObject::connect(ui.helpView, SIGNAL(forwardAvailable(bool)), ui.actionGoForward, SLOT(setEnabled(bool)));
    QObject::connect(ui.helpView, SIGNAL(backwardAvailable(bool)), ui.actionGoBackward, SLOT(setEnabled(bool)));
    QObject::connect(ui.helpView, SIGNAL(upAvailable(bool)), ui.actionGoUp, SLOT(setEnabled(bool)));
    QObject::connect(ui.actionGoForward, SIGNAL(triggered()), ui.helpView, SLOT(forward()));
    QObject::connect(ui.actionGoBackward, SIGNAL(triggered()), ui.helpView, SLOT(backward()));
    QObject::connect(ui.actionGoHome, SIGNAL(triggered()), ui.helpView, SLOT(home()));
    QObject::connect(ui.actionCopy, SIGNAL(triggered()), ui.helpView, SLOT(copy()));
    QObject::connect(ui.actionGoUp, SIGNAL(triggered()), ui.helpView, SLOT(up()));
    QObject::connect(ui.bookmarkDelete, SIGNAL(clicked()), ui.bookmarkList, SLOT(deleteSelected()));
    QObject::connect(ui.bookmarkAdd, SIGNAL(clicked()), ui.actionAddBookmark, SLOT(trigger()));
    QObject::connect(ui.bookmarkList, SIGNAL(urlActivated(QUrl)), ui.helpView, SLOT(setSource(QUrl)));

    q->connect(ui.actionAbout, SIGNAL(triggered()), SLOT(aboutDialog()));
    q->connect(ui.actionPreferences, SIGNAL(triggered()), SLOT(prefsDialog()));
    q->connect(ui.actionClose, SIGNAL(triggered()), SLOT(close()));
    q->connect(ui.actionQuit, SIGNAL(triggered()), SLOT(quit()));
    q->connect(ui.actionPrint, SIGNAL(triggered()), SLOT(printDocument()));
    q->connect(ui.actionPageSetup, SIGNAL(triggered()), SLOT(pageSetup()));
    q->connect(ui.actionFind, SIGNAL(triggered()), SLOT(startFind()));
    q->connect(ui.actionFindNext, SIGNAL(triggered()), SLOT(findNext()));
    q->connect(ui.actionFindPrev, SIGNAL(triggered()), SLOT(findPrev()));
    q->connect(ui.findButtonNext, SIGNAL(clicked()), SLOT(findNext()));
    q->connect(ui.findButtonPrev, SIGNAL(clicked()), SLOT(findPrev()));
    q->connect(ui.findButtonClose, SIGNAL(clicked()), SLOT(cancelFind()));
    q->connect(ui.actionPrintPreview, SIGNAL(triggered()), SLOT(printPreview()));
    q->connect(ui.findEntry, SIGNAL(textEdited(QString)), SLOT(findEntryTextEdited(QString)));
    q->connect(ui.helpView, SIGNAL(sourceChanged(QUrl)), SLOT(viewSourceChanged(QUrl)));

    ui.findBox->hide();
    ui.findEntry->installEventFilter(q);

    ui.toolbarAddress->hide();
    addressEntry = new QComboBox(ui.toolbarAddress);
    addressEntry->setEditable(true);
    addressEntry->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.toolbarAddress->insertWidget(ui.actionGo, addressEntry);
    pub->connect(addressEntry, SIGNAL(activated(QString)), SLOT(openAddress(QString)));

    ui.menubar->insertMenu(ui.menuHelp->menuAction(), q->windowMenu());

    ui.bookmarkDock->hide();
    QAction *action = ui.bookmarkDock->toggleViewAction();
    action->setShortcut(QKeySequence("Alt+B"));
    ui.menuView->addAction(action);

    ui.menuView->addAction(ui.toolbar->toggleViewAction());
    ui.menuView->addAction(ui.toolbarAddress->toggleViewAction());

    q->setActionShortcut("actionGoForward", QKeySequence::Forward);
    q->setActionShortcut("actionGoBackward", QKeySequence::Back);
    q->loadUiConfig();
}

void HelpBrowser::startFind()
{
    impl->ui.findBox->show();
    impl->ui.findEntry->setFocus(Qt::OtherFocusReason);
    impl->ui.findEntry->selectAll();
}

void HelpBrowser::cancelFind()
{
    bool had_focus = impl->ui.findEntry->hasFocus();
    impl->ui.findBox->hide();
    if (had_focus)
        impl->ui.helpView->setFocus(Qt::OtherFocusReason);
}

void HelpBrowser::doFind(const QString &text, bool forward, bool atCursorOk)
{
    QTextDocument::FindFlags flags = 0;
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;

    if (!forward)
        flags |= QTextDocument::FindBackward;
    if (impl->ui.findCheckCaseSensitive->isChecked())
    {
        flags |= QTextDocument::FindCaseSensitively;
        cs = Qt::CaseSensitive;
    }
    if (impl->ui.findCheckWholeWords->isChecked())
        flags |= QTextDocument::FindWholeWords;

    int start_pos = -1;
    QTextCursor cr = view()->textCursor();

    if (cr.hasSelection())
    {
        QString s = cr.selectedText();
        if (forward)
        {
            if (s.startsWith(text) && QString::compare(s, text, cs) != 0)
            {
                cr.setPosition(cr.selectionStart());
                cr.setPosition(cr.position() + text.length(), QTextCursor::KeepAnchor);
                view()->setTextCursor(cr);
                view()->ensureCursorVisible();
                return;
            }

            if (text.startsWith(s, cs) && QString::compare(s, text, cs) != 0)
                start_pos = cr.selectionStart();
            else
                start_pos = cr.selectionStart() + 1;
        }
        else
        {
            if (s.endsWith(text) && QString::compare(s, text, cs) != 0)
            {
                cr.setPosition(cr.selectionEnd());
                cr.setPosition(cr.position() - text.length(), QTextCursor::KeepAnchor);
                view()->setTextCursor(cr);
                view()->ensureCursorVisible();
                return;
            }

            if (text.endsWith(s, cs) && QString::compare(s, text, cs) != 0)
                start_pos = cr.selectionStart();
            else
                start_pos = cr.selectionStart() - 1;
        }
    }
    else
    {
        start_pos = cr.position();
    }

    QTextCursor res;

    if (forward)
    {
        res = view()->document()->find(text, start_pos, flags);
        if (res.isNull())
            res = view()->document()->find(text, 0, flags);
    }
    else
    {
        if (start_pos >= 0)
            res = view()->document()->find(text, start_pos, flags);
        if (res.isNull())
        {
            QTextCursor tmp(view()->document());
            tmp.movePosition(QTextCursor::End);
            res = view()->document()->find(text, tmp.position(), flags);
        }
    }

    if (res.isNull())
    {
        QApplication::beep();
    }
    else
    {
        view()->setTextCursor(res);
        view()->ensureCursorVisible();
    }
}

void HelpBrowser::findEntryTextEdited(const QString &text)
{
    doFind(text, true, true);
}

void HelpBrowser::findNext()
{
    QString text = impl->ui.findEntry->text();
    if (text.isEmpty())
        return;
    doFind(text, true, false);
}

void HelpBrowser::findPrev()
{
    QString text = impl->ui.findEntry->text();
    if (text.isEmpty())
        return;
    doFind(text, false);
}

bool HelpBrowser::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != impl->ui.findEntry || event->type() != QEvent::KeyPress)
        return QMainWindow::eventFilter(obj, event);

    QKeyEvent *ke = static_cast<QKeyEvent*>(event);
    switch (ke->key())
    {
        case Qt::Key_Escape:
            cancelFind();
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            findNext();
            return true;
        default:
            return QMainWindow::eventFilter(obj, event);
    }
}

HelpView *HelpBrowser::view()
{
    return impl->ui.helpView;
}

void HelpBrowser::on_actionAddBookmark_triggered()
{
    QString label = impl->ui.helpView->documentTitle();
    QUrl url = impl->ui.helpView->source();

    if (!url.isValid())
        return;

    label = runBookmarkDialog(label, this);
    if (!label.isEmpty())
        impl->ui.bookmarkList->addBookmark(label, url);
}

void HelpBrowser::on_actionGoNextPage_triggered()
{
    implement_me();
}

void HelpBrowser::on_actionGoPrevPage_triggered()
{
    implement_me();
}

void HelpBrowser::openAddress(const QString &text)
{
    if (text.isEmpty())
        return;
    QUrl url(text, QUrl::TolerantMode);
    if (!url.isValid())
    {
        informationDialog(this, QString(), "Invalid URL", QString(url.errorString()));
        return;
    }
    view()->setSource(url);
}

void HelpBrowser::viewSourceChanged(const QUrl &url)
{
    impl->addressEntry->setEditText(url.toString());
}

void HelpBrowser::on_actionGo_triggered()
{
    openAddress(impl->addressEntry->currentText());
}


void HelpBrowser::applyPrefs()
{
    if (HelpBrowserPrivate::instance)
        HelpBrowserPrivate::instance->doApplyPrefs();
}

void HelpBrowser::doApplyPrefs()
{
    view()->applyPrefs();
}


static void zoom(QWidget *w, double factor)
{
    QFont font = w->font();
    if (font.pointSize() > 0)
        font.setPointSize(qBound(6., factor * font.pointSize(), 10000.));
    else if (font.pixelSize() > 0)
        font.setPixelSize(qBound(6., factor * font.pixelSize(), 10000.));
    else
        qCritical("%s: oops", Q_FUNC_INFO);

    w->setFont(font);
}

void HelpBrowser::on_actionZoomIn_triggered()
{
    zoom(view(), 1.2);
}

void HelpBrowser::on_actionZoomOut_triggered()
{
    zoom(view(), 1./1.2);
}

void HelpBrowser::on_actionZoomNormal_triggered()
{
    view()->setFontFromPrefs();
}
