#ifndef MOO_WS_TEXT_DOCUMENT_H
#define MOO_WS_TEXT_DOCUMENT_H

#include <QTextDocument>
#include <QTextCursor>
#include <moo-pimpl.h>

namespace moo {
namespace ws {

class Block;
class TextDocumentPrivate;
class EditCommand;

namespace rt {
class Fragment;
}

class TextDocument : public QObject {
public:
    TextDocument(QObject *parent = 0);
    ~TextDocument();

    QTextDocument *qdoc();
    const QTextDocument *qdoc() const;
    QTextCursor cursor(int position = 0);

    Block *firstBlock();
    Block *lastBlock();

    void insertBlock(Block *block, Block *after);
    void appendBlock(Block *block);
    void deleteBlock(Block *block);

    void insertText(QTextCursor &cr, const QString &text, const QTextCharFormat *format);
    void insertFragment(QTextCursor &cr, const rt::Fragment &fragment);
    void deleteText(QTextCursor &cr);
    void applyFormat(QTextCursor &cr, const QTextCharFormat &format);

    enum DeleteType {
        DeletePreviousChar = 1,
        DeleteNextChar,
        DeleteEndOfWord,
        DeleteStartOfWord,
        DeleteEndOfLine,
        DeleteSelected
    };
    bool insertInteractive(QTextCursor &cr, const QString &text,
                           const QTextCharFormat *fmt,
                           bool overwriteMode = false);
    bool deleteInteractive(QTextCursor &cr, DeleteType type);
    bool deleteBlocksInteractive(QTextCursor &cr);

    void beginAction();
    void endAction();
    bool isRedoAvailable() const;
    bool isUndoAvailable() const;
    bool isUndoRedoEnabled() const;
    void setUndoRedoEnabled(bool enable);
    void undo(QTextCursor *cr);
    void redo(QTextCursor *cr);
    bool isModified () const;

    void setHrefParser(QString (*parse) (const QString&));

public Q_SLOTS:
    void undo();
    void redo();
    void setModified(bool modified = true);

Q_SIGNALS:
    void beep();
    void contentsChanged();
    void modificationChanged(bool modified);
    void redoAvailable(bool available);
    void undoAvailable(bool available);

private:
    Q_OBJECT
    M_DECLARE_IMPL(TextDocument)
    friend class EditCommand;

    bool replaceSelectedInteractive(QTextCursor &cr, const QString &text,
                                    const QTextCharFormat *format);
    void deleteBlock(Block *block, bool destroying);
    void clearUndoRedo();
    void check();
};

inline QTextCursor TextDocument::cursor(int position)
{
    QTextCursor cr(qdoc());
    if (position > 0)
    {
        cr.setPosition(position);
    }
    else if (position < 0)
    {
        cr.movePosition(QTextCursor::End);
        if (position < -1)
            cr.setPosition(cr.position() + position + 1);
    }
    return cr;
}

} // namespace ws
} // namespace moo

#endif // MOO_WS_TEXT_DOCUMENT_H
