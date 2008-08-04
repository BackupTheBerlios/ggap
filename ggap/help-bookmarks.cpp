#include "ggap/help-bookmarks-p.h"
#include "ggap/utils.h"
#include "ggap/prefs.h"
#include "ggap/ui_addbookmark.h"
#include <QDialog>
#include <moo-macros.h>

using namespace ggap;

BookmarkList::BookmarkList(QWidget *parent) :
    QListView(parent),
    impl(this)
{
    setModel(impl->model);
    setEditTriggers(NoEditTriggers);
    connect(this, SIGNAL(activated(QModelIndex)), SLOT(activated(QModelIndex)));
}

BookmarkList::~BookmarkList()
{
}

BookmarkListPrivate::BookmarkListPrivate(BookmarkList *q) :
    pub(q),
    model(BookmarkStore::instance()->model())
{
}

BookmarkListPrivate::~BookmarkListPrivate()
{
}

void BookmarkList::activated(const QModelIndex &index)
{
    emit urlActivated(impl->model->url(index));
}

void BookmarkList::deleteSelected()
{
    const QModelIndexList list = selectedIndexes();
    if (list.size() != 1)
        return;
    const QModelIndex idx = list.first();
    impl->model->removeRow(idx.row());
    BookmarkStore::instance()->save();
}

void BookmarkList::addBookmark(const QString &label, const QUrl &url)
{
    BookmarkItem *bi = new BookmarkItem(label, url);
    impl->model->appendRow(bi);
    BookmarkStore::instance()->save();
}

void BookmarkList::activateBookmark(int row)
{
    QStandardItem *item = impl->model->item(row);
    BookmarkItem *bi = dynamic_cast<BookmarkItem*>(item);
    m_return_if_fail(bi != 0);
    emit urlActivated(bi->url());
}


BookmarkItem::BookmarkItem(const QString &label, const QUrl &url) :
    QStandardItem(label),
    url_(url)
{
}


QUrl BookmarkModel::url(const QModelIndex &index) const
{
    QStandardItem *item = itemFromIndex(index);
    BookmarkItem *bi = dynamic_cast<BookmarkItem*>(item);
    m_return_val_if_fail(bi != 0, QUrl());
    return bi->url();
}


BookmarkStore *BookmarkStore::instance()
{
    static util::DataOnDemand<BookmarkStore> store;
    return store;
}

BookmarkStore::BookmarkStore()
{
    load();
}

void BookmarkStore::add(const QString &label, const QString &url)
{
    BookmarkItem *bi = new BookmarkItem(label, url);
    model_.appendRow(bi);
}

void BookmarkStore::load()
{
    QStringList list = prefsValue(Prefs::HelpBookmarks);
    for (int i = 0; i + 1 < list.size(); i += 2)
        add(list[i], list[i+1]);
}

void BookmarkStore::save()
{
    QStringList strings;
    QList<QStandardItem*> items = model_.findItems("", Qt::MatchContains);
    for (int i = 0; i < items.size(); ++i)
        if (BookmarkItem *bi = dynamic_cast<BookmarkItem*>(items[i]))
            strings << bi->text() << bi->url().toString();
    setPrefsValue(Prefs::HelpBookmarks, strings);

    foreach (BookmarkMenu *m, menus)
        m->updateMenu();
}


QString ggap::runBookmarkDialog(const QString &label, QWidget *parent)
{
    QDialog dlg(parent);
    Ui::AddBookmarkDialog ui;
    ui.setupUi(&dlg);
    ui.entryLabel->setText(label);
    ui.entryLabel->selectAll();

    if (dlg.exec() != QDialog::Accepted)
        return QString();
    else
        return ui.entryLabel->text();
}


QMenu *ggap::createBookmarkMenu(QAction *actionAddBookmark, QWidget *parent)
{
    return new BookmarkMenu(actionAddBookmark, parent);
}

BookmarkMenu::BookmarkMenu(QAction *actionAddBookmark, QWidget *parent) :
    QMenu(parent),
    separator(0),
    mapper(0)
{
    setObjectName("menuBookmarks");
    setTitle(tr("&Bookmarks"));

    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), this, SIGNAL(itemActivated(int)));

    addAction(actionAddBookmark);
    separator = addSeparator();

    updateMenu();
    BookmarkStore::instance()->addMenu(this);
}

BookmarkMenu::~BookmarkMenu()
{
    BookmarkStore::instance()->removeMenu(this);
}

void BookmarkMenu::updateMenu()
{
    while (!actions.isEmpty())
        removeAction(actions.takeLast());

    BookmarkModel *model = BookmarkStore::instance()->model();
    QList<QStandardItem*> items = model->findItems("", Qt::MatchContains);
    for (int i = 0; i < items.size(); ++i)
        if (BookmarkItem *bi = dynamic_cast<BookmarkItem*>(items[i]))
        {
            QAction *a = addAction(bi->text());
            if (i < 9)
                a->setShortcut(QKeySequence(QString("Ctrl+%1").arg(i+1)));
            else if (i == 9)
                a->setShortcut(QKeySequence("Ctrl+0"));
            mapper->setMapping(a, i);
            mapper->connect(a, SIGNAL(triggered()), SLOT(map()));
            actions << a;
        }

    separator->setVisible(!actions.isEmpty());
}
