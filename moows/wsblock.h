#ifndef MOO_WS_BLOCK_H
#define MOO_WS_BLOCK_H

#include <moo-pimpl.h>
#include <moows/wsblockformat.h>
#include <moows/worksheet-xml.h>
#include <QTextCursor>
#include <QTextBlockUserData>

namespace moo {
namespace ws {

class TextDocument;
class Worksheet;
class WorksheetPrivate;
class EditCommand;
class BlockPrivate;

class Block : public QObject {
    M_DECLARE_IMPL(Block)

protected:
    Block();
    virtual ~Block();

protected:
    virtual void added() = 0;
    virtual void removed() = 0;

    void setEditable(bool editable);
    void insertText(QTextCursor &cr, const QString &text);
    void insertFragment(QTextCursor &cr, const rt::Fragment &fragment);
    void setText(const QString &text);

public:

    void setFormat(const BlockFormat &fmt);
    const BlockFormat &format() const;
    bool isEditable() const;

    QTextCursor startCursor() const;
    QTextCursor endCursor() const;
    QTextCursor cursorAtLine(int n) const;

    static Block *atCursor(const QTextCursor &cr);
    static Block *atPosition(TextDocument *doc, int position);
    static Block *fromUserData(QTextBlockUserData *ud);

    TextDocument *document();
    Block *next();
    Block *prev();
    const TextDocument *document() const;
    const Block *next() const;
    const Block *prev() const;

private:
    friend class TextDocument;
    friend class TextDocumentPrivate;
    friend class EditCommand;
};

} // namespace ws
} // namespace moo

#endif // MOO_WS_BLOCK_H
