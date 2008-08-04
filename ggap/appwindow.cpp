#include "moo-macros.h"
#include "ggap/app.h"
#include "ggap/appwindow.h"
#include "ggap/utils.h"
#include <QtGui>

using namespace ggap;

static QList<AppWindow*> window_list;

QList<AppWindow*> AppWindow::listWindows()
{
    return window_list;
}

AppWindow::AppWindow(const QString &name) :
    window_menu(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose);
    setObjectName(name);

// #ifndef Q_OS_MAC
//     setWindowIcon(QIcon(":/icons/ggap.png"));
// #endif

    WindowMenu::queueUpdateMenus();
    window_menu = new WindowMenu(this, this);
    window_list << this;
}

AppWindow::~AppWindow()
{
    window_list.removeAll(this);
    WindowMenu::queueUpdateMenus();
}

WindowMenu *AppWindow::windowMenu()
{
    return window_menu;
}

static QString windowSizeKey(AppWindow *w)
{
    return QString("ui/window-size-") + w->objectName();
}

static QString windowStateKey(AppWindow *w)
{
    return QString("ui/window-state-") + w->objectName();
}


static QAction *findAction(AppWindow *self, const char *name)
{
    QAction *a = self->findChild<QAction*>(name);
    if (!a)
        qCritical("no action `%s' in window `%s'", name,
                  self->objectName().toLocal8Bit().data());
    return a;
}

void AppWindow::setActionShortcut(const char *name, const QKeySequence &shortcut)
{
    if (QAction *a = findAction(this, name))
        a->setShortcut(shortcut);
}

void AppWindow::connectAction(const char *name, const char *slot)
{
    if (QAction *a = findAction(this, name))
        connect(a, SIGNAL(triggered()), slot);
}

void AppWindow::connectAction(QAction *action, const char *slot)
{
    connect(action, SIGNAL(triggered()), slot);
}

void AppWindow::connectAction(const char *name, QObject *receiver, const char *slot)
{
    if (QAction *a = findAction(this, name))
        connect(a, SIGNAL(triggered()), receiver, slot);
}

void AppWindow::connectAction(QAction *action, QObject *receiver, const char *slot)
{
    connect(action, SIGNAL(triggered()), receiver, slot);
}

void AppWindow::loadUiConfig()
{
    connectAction("actionClose", SLOT(close()));
    connectAction("actionMinimize", SLOT(showMinimized()));
    connectAction("actionZoom", SLOT(zoomWindow()));
    connectAction("actionBringAllToFront", SLOT(bringAllToFront()));
    connectAction("actionAbout", gapApp, SLOT(aboutDialog()));
    connectAction("actionAboutQt", gapApp, SLOT(aboutQt()));
    connectAction("actionPrefs", gapApp, SLOT(prefsDialog()));
    connectAction("actionQuit", gapApp, SLOT(maybeQuit()));

    QSize size;

#ifndef Q_OS_WIN32
    size = stateValue(windowSizeKey(this), QSize()).toSize();
#endif
    if (size.width() > 0 && size.height() > 0)
        resize(size);

    QByteArray state = stateValue(windowStateKey(this), QByteArray()).toByteArray();
    if (!state.isEmpty())
        restoreState(state);
}

void AppWindow::saveUiConfig()
{
    setStateValue(windowSizeKey(this), size());
    setStateValue(windowStateKey(this), saveState());
}

void AppWindow::closeEvent(QCloseEvent *e)
{
    saveUiConfig();
    e->accept();
}

static void show_implement_me(QWidget *parent, const char *func)
{
    QMessageBox::warning(parent, QString(), QString("IMPLEMENT ME\n%1").arg(func));
}

#define implement_me() show_implement_me(this, __func__)

void AppWindow::bringAllToFront()
{
    implement_me();
}

void AppWindow::zoomWindow()
{
    if (isMaximized())
        showNormal();
    else
        showMaximized();
}

void AppWindow::setWindowFilePath(const QString &path)
{
#if QT_VERSION >= 0x040400
    QMainWindow::setWindowFilePath(path);
    emit windowFilePathChanged(path);
#else
    setWindowTitle(path);
#endif
}

void AppWindow::setWindowTitle(const QString &title)
{
    QMainWindow::setWindowTitle(title);
    emit windowTitleChanged(title);
}

void AppWindow::setWindowModified(bool modified)
{
    QMainWindow::setWindowModified(modified);
    emit windowModifiedChanged(modified);
}


TextEditWindow::TextEditWindow(const QString &name) :
    AppWindow(name)
{
}

TextEditWindow::~TextEditWindow()
{
}

void TextEditWindow::loadUiConfig()
{
    connectAction("actionPrint", SLOT(printDocument()));
    connectAction("actionPageSetup", SLOT(pageSetup()));
    connectAction("actionPrintPreview", SLOT(printPreview()));

#if defined(Q_OS_MAC) || QT_VERSION < 0x040400
    if (QAction *a = findAction(this, "actionPrintPreview"))
        a->setVisible(false);
#endif

    AppWindow::loadUiConfig();
}


///////////////////////////////////////////////////////////////////////////
//
// Printing
//

namespace {

class PageSetup {
    bool isValid;
    QPrinter::Orientation orientation;
    QPrinter::PageSize size;

public:
    PageSetup(QPrinter *printer) :
        isValid(false),
        orientation(QPrinter::Portrait),
        size(QPrinter::A4)
    {
        m_return_if_fail(printer != 0);
        orientation = printer->orientation();
        size = printer->pageSize();
        isValid = true;
    }

    void apply(QPrinter *printer)
    {
        m_return_if_fail(printer != 0);
        if (isValid)
        {
            printer->setPageSize(size);
            printer->setOrientation(orientation);
        }
    }

    PageSetup(const QByteArray &data) :
        isValid(false),
        orientation(QPrinter::Portrait),
        size(QPrinter::A4)
    {
        QDataStream s(data);
        QMap<QString, QVariant> map;
        s >> map;
        QString version = map["version"].toString();
        if (version != "1.0")
        {
            qCritical("%s: oops", Q_FUNC_INFO);
            return;
        }
        orientation = QPrinter::Orientation(map["orientation"].toInt());
        size = QPrinter::PageSize(map["size"].toInt());
        isValid = true;
    }

    QByteArray save() const
    {
        if (!isValid)
            return QByteArray();

        QMap<QString, QVariant> map;
        map["version"] = QString("1.0");
        map["orientation"] = int(orientation);
        map["size"] = int(size);
        QByteArray data;
        QDataStream ds(&data, QIODevice::WriteOnly);
        ds << map;
        return data;
    }
};

static QPrinter *createPrinter()
{
    QPrinter *printer = new QPrinter;
    QByteArray data = prefsValue(Prefs::PageSetup);

    if (!data.isEmpty())
    {
        PageSetup ps(data);
        ps.apply(printer);
    }

    return printer;
}

static ggap::util::DataOnDemand<QPrinter> global_printer(createPrinter);

}

static void savePrinter()
{
    m_return_if_fail(global_printer);
    PageSetup ps(global_printer);
    setPrefsValue(Prefs::PageSetup, ps.save());
}

void TextEditWindow::printDocument()
{
    QTextEdit *doc = textEdit();
    m_return_if_fail(doc != 0);

    QPrintDialog dlg(global_printer, this);
    if (doc->textCursor().hasSelection())
        dlg.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (dlg.exec() != QDialog::Accepted)
        return;

    doc->print(global_printer);

    savePrinter();
}

void TextEditWindow::previewPaintRequested(QPrinter *printer)
{
    QTextEdit *doc = textEdit();
    m_return_if_fail(doc != 0);
    doc->print(printer);
}

void TextEditWindow::printPreview()
{
#if QT_VERSION >= 0x040400
    QPrintPreviewDialog dlg(global_printer, this);
    connect(&dlg, SIGNAL(paintRequested(QPrinter*)),
            SLOT(previewPaintRequested(QPrinter*)));
    dlg.exec();
#endif
}

void TextEditWindow::pageSetup()
{
    QPageSetupDialog dlg(global_printer, this);
    if (dlg.exec() == QDialog::Accepted)
        savePrinter();
}
