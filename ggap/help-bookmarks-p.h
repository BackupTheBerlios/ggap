#ifndef GGAP_HELP_BOOKMARKS_P_H
#define GGAP_HELP_BOOKMARKS_P_H

#include <QStandardItem>
#include <QStandardItemModel>
#include <QSignalMapper>
#include <QUrl>
#include <QMenu>
#include "ggap/help-bookmarks.h"

namespace ggap {

class BookmarkItem : public QStandardItem {
    QUrl url_;

public:
    BookmarkItem(const QString &label, const QUrl &url);
    const QUrl &url() const { return url_; }
};

class BookmarkModel : public QStandardItemModel {
public:
    QUrl url(const QModelIndex &index) const;
};

class BookmarkMenu : public QMenu {
    Q_OBJECT

    QAction *separator;
    QList<QAction*> actions;
    QSignalMapper *mapper;

public:
    BookmarkMenu(QAction *actionAddBookmark, QWidget *parent);
    ~BookmarkMenu();
    void updateMenu();

Q_SIGNALS:
    void itemActivated(int idx);
};

class BookmarkStore : public QObject {
    Q_OBJECT
    friend class BookmarkList;

    BookmarkModel model_;
    QList<BookmarkMenu*> menus;

    void add(const QString &label, const QString &url);
    void load();
    void save();

public:
    static BookmarkStore *instance();

    BookmarkStore();
    BookmarkModel *model() { return &model_; }

    void addMenu(BookmarkMenu *menu)
    {
        menus.append(menu);
    }

    void removeMenu(BookmarkMenu *menu)
    {
        menus.removeAll(menu);
    }
};

class BookmarkListPrivate {
    M_DECLARE_PUBLIC(BookmarkList)

    BookmarkModel *model;

public:
    BookmarkListPrivate(BookmarkList *q);
    ~BookmarkListPrivate();
};

}

#endif // GGAP_HELP_BOOKMARKS_P_H
