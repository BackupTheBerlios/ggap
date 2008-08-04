#ifndef WS_TEXT_DOCUMENT_P_H
#define WS_TEXT_DOCUMENT_P_H

#include "moows/wstextdocument.h"
#include "moows/wsblock-format.h"
#include <QUndoStack>

namespace moo {
namespace ws {

class Worksheet;
class UndoCommand;

class UndoStack : public QObject {
    Q_OBJECT

    QUndoStack qstack;
    bool ext_modified;
    int group;
    bool in_macro;
    QString macro_name;

    void endMacro()
    {
        if (in_macro)
        {
            in_macro = false;
            qstack.endMacro();
        }
    }

public:
    UndoStack(QObject *parent = 0) :
        QObject(parent),
        ext_modified(false),
        group(0),
        in_macro(false)
    {
        connect(&qstack, SIGNAL(canUndoChanged(bool)), SIGNAL(canUndoChanged(bool)));
        connect(&qstack, SIGNAL(canRedoChanged(bool)), SIGNAL(canRedoChanged(bool)));
    }

    void beginGroup(const QString &name = QString())
    {
        if (!group)
            macro_name = name;
        group++;
    }

    void endGroup()
    {
        if (!group)
        {
            qCritical("%s: oops", Q_FUNC_INFO);
            return;
        }
        if (!--group)
            endMacro();
    }

    bool canUndo() const
    {
        return qstack.canUndo();
    }

    bool canRedo() const
    {
        return qstack.canRedo();
    }

    bool isModified() const
    {
        return ext_modified || !qstack.isClean();
    }

    void setModified(bool modified)
    {
        if (modified == isModified())
            return;

        ext_modified = modified;
        if (!modified)
            qstack.setClean();

        emit modificationChanged(isModified());
    }

    void clear()
    {
        bool was_modified = isModified();
        endMacro();
        qstack.clear();
        if (was_modified != isModified())
            emit modificationChanged(isModified());
    }

    void push(QUndoCommand *cmd)
    {
        bool was_modified = isModified();

        if (group && !in_macro)
        {
            in_macro = true;
            qstack.beginMacro(macro_name);
        }

        qstack.push(cmd);

        if (was_modified != isModified())
            emit modificationChanged(isModified());
    }

    void undo()
    {
        bool was_modified = isModified();
        endMacro();
        qstack.undo();
        if (was_modified != isModified())
            emit modificationChanged(isModified());
    }

    void redo()
    {
        bool was_modified = isModified();
        endMacro();
        qstack.redo();
        if (was_modified != isModified())
            emit modificationChanged(isModified());
    }

Q_SIGNALS:
    void modificationChanged(bool);
    void canUndoChanged(bool);
    void canRedoChanged(bool);
};

class TextDocumentPrivate {
    friend class Worksheet;
    friend class EditCommand;

protected:
    M_DECLARE_PUBLIC(TextDocument)

    TextDocumentPrivate(TextDocument *doc);
    ~TextDocumentPrivate();

private:
    void insertText(QTextCursor &cr, const QString &text, const QTextCharFormat *format);
    void insertFragment(QTextCursor &cr, const rt::Fragment &fragment);
    void deleteText(QTextCursor &cr);
    void applyFormat(QTextCursor &cr, const QTextCharFormat &format);

    void undoRedo(bool undo, QTextCursor *cr);

    void nonUndoableModification();
    // apply format
    void modify(const QTextCursor &cr, const QTextCharFormat &fmt, bool undoable);
    // insert
    void modify(const QString &text, const QTextCursor &cr, bool undoable);
    void modify(const rt::Fragment &fragment, const QTextCursor &cr, bool undoable);
    // delete
    void modify(const QTextCursor &cr, bool undoable);
    void modify(UndoCommand *cmd, bool undoable);

    QString parseHref(const QString &href)
    {
        if (hrefParser && !href.isEmpty())
            return hrefParser(href);
        else
            return href;
    }

private:
    ColorScheme colorScheme;
    Block *first;
    Block *last;
    QTextDocument qdoc;
    UndoStack undo_stack;
    QTextCursor *undo_cursor;
    QString (*hrefParser) (const QString &href);
};

}
}

#endif // WS_TEXT_DOCUMENT_P_H
