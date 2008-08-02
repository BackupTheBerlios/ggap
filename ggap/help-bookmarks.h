#ifndef GGAP_HELP_BOOKMARKS_H
#define GGAP_HELP_BOOKMARKS_H

#include <moo-pimpl.h>
#include <QListView>
#include <QString>

class QUrl;
class QMenu;

namespace ggap {

class BookmarkListPrivate;
class BookmarkList : public QListView {
    Q_OBJECT
    M_DECLARE_IMPL(BookmarkList)

public:
    BookmarkList(QWidget *parent = 0);
    ~BookmarkList();

    void addBookmark(const QString &label, const QUrl &url);

public Q_SLOTS:
    void deleteSelected();
    void activateBookmark(int idx);

private Q_SLOTS:
    void activated(const QModelIndex &index);

Q_SIGNALS:
    void urlActivated(const QUrl &url);
};

QString runBookmarkDialog(const QString &label, QWidget *parent);
QMenu *createBookmarkMenu(QAction *actionAddBookmark, QWidget *parent);

}

#endif // GGAP_HELP_H
