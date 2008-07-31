#include "moows/wstextdocument_p.h"
#include "moows/wsblock_p.h"
#include "moows/worksheet-xml.h"
#include "moows/wstextblock.h"
#include "moo-macros.h"
#include <QPointer>
#include <QDebug>

#undef ENABLE_DEBUG_CHECK

using namespace moo::ws;

TextDocumentPrivate::TextDocumentPrivate(TextDocument *doc) :
    pub(doc),
    first(0), last(0),
    undo_cursor(0),
    hrefParser(0)
{
}

TextDocumentPrivate::~TextDocumentPrivate()
{
}

TextDocument::TextDocument(QObject *parent) :
    QObject(parent), impl(this)
{
    impl->qdoc.setUndoRedoEnabled(false);
    connect(&impl->qdoc, SIGNAL(contentsChanged()), SIGNAL(contentsChanged()));
    connect(&impl->undo_stack, SIGNAL(modificationChanged(bool)), SIGNAL(modificationChanged(bool)));
    connect(&impl->undo_stack, SIGNAL(canRedoChanged(bool)), SIGNAL(redoAvailable(bool)));
    connect(&impl->undo_stack, SIGNAL(canUndoChanged(bool)), SIGNAL(undoAvailable(bool)));
}

TextDocument::~TextDocument()
{
    while (impl->first)
        deleteBlock(impl->first, true);
}

QTextDocument *TextDocument::qdoc()
{
    return &impl->qdoc;
}

const QTextDocument *TextDocument::qdoc() const
{
    return &impl->qdoc;
}

void TextDocument::insertBlock(Block *block, Block *after)
{
    Block *before;

    m_return_if_fail(block != 0);

    impl->nonUndoableModification();

    if (after)
        before = after->impl->next;
    else
        before = impl->first;

    QTextCursor cr(qdoc());
    cr.beginEditBlock();

    QTextBlock tb;

    QTextBlockFormat block_fmt = block->format().blockFormat();

    if (after && before)
    {
        cr = before->impl->start;
        cr.insertBlock(block_fmt);
        cr.block().setUserData(new WsUserData(before));
        cr.movePosition(QTextCursor::PreviousBlock);
        tb = cr.block();
    }
    else if (after)
    {
        cr = after->impl->end;
        cr.insertBlock(block_fmt);
        tb = cr.block();
        after->impl->end.movePosition(QTextCursor::PreviousBlock);
        if (after->impl->start > after->impl->end)
            after->impl->start.movePosition(QTextCursor::PreviousBlock);
        if (!after->impl->end.atBlockEnd())
            after->impl->end.movePosition(QTextCursor::EndOfBlock);
    }
    else if (before)
    {
        cr = after->impl->start;
        cr.insertBlock(block_fmt);
        cr.block().setUserData(new WsUserData(before));
        cr.movePosition(QTextCursor::PreviousBlock);
        tb = cr.block();
    }
    else
    {
        cr = cursor();
        tb = cr.block();
        cr.setBlockFormat(block_fmt);
    }

    tb.setUserData(new WsUserData(block));

    block->impl->doc = this;
    block->impl->start = cr;
    block->impl->end = cr;
    block->impl->next = before;
    block->impl->prev = after;
    if (before)
        before->impl->prev = block;
    if (after)
        after->impl->next = block;
    if (!block->impl->prev)
        impl->first = block;
    if (!block->impl->next)
        impl->last = block;

    block->added();

    cr.endEditBlock();

    check();
}

void TextDocument::appendBlock(Block *block)
{
    insertBlock(block, impl->last);
}

void TextDocument::deleteBlock(Block *block)
{
    deleteBlock(block, false);
}

void TextDocument::deleteBlock(Block *block, bool destroying)
{
    m_return_if_fail(block != 0);
    m_return_if_fail(block->impl->doc == this);

    impl->nonUndoableModification();

    block->removed();

    Block *prev = block->impl->prev;
    Block *next = block->impl->next;

    if (!destroying)
    {
        QTextCursor cr, end;
        bool unset_user_data = false;

        if (next)
        {
            cr = block->impl->start;
            end = next->impl->start;
        }
        else if (prev)
        {
            cr = prev->impl->end;
            end = block->impl->end;
        }
        else
        {
            cr = block->impl->start;
            end = block->impl->end;
            unset_user_data = true;
        }

        cr.setPosition(end.position(), QTextCursor::KeepAnchor);
        cr.removeSelectedText();

        if (unset_user_data)
        {
            M_ASSERT(cr.block().userData() != 0);
            cr.block().setUserData(0);
        }
    }

    if (block == impl->first)
        impl->first = next;
    if (block == impl->last)
        impl->last = prev;
    if (next)
        next->impl->prev = prev;
    if (prev)
        prev->impl->next = next;

    delete block;

    if (!destroying)
        check();
}

void TextDocument::insertText(QTextCursor &cr, const QString &text, const QTextCharFormat *format)
{
    impl->insertText(cr, text, format);
}

void TextDocument::insertFragment(QTextCursor &cr, const rt::Fragment &fragment)
{
    impl->insertFragment(cr, fragment);
}

void TextDocument::deleteText(QTextCursor &cr)
{
    impl->deleteText(cr);
}

void TextDocument::applyFormat(QTextCursor &cr, const QTextCharFormat &format)
{
    impl->applyFormat(cr, format);
}

Block *TextDocument::firstBlock()
{
    return impl->first;
}

Block *TextDocument::lastBlock()
{
    return impl->last;
}

bool TextDocument::replaceSelectedInteractive(QTextCursor &cr, const QString &text,
                                              const QTextCharFormat *format)
{
    if (!cr.hasSelection())
        return insertInteractive(cr, text, format, false);

    bool ret = false;
    beginAction();

    if (deleteInteractive(cr, DeleteSelected))
    {
        insertInteractive(cr, text, format, false);
        ret = true;
    }

    endAction();
    return ret;
}

bool TextDocument::insertInteractive(QTextCursor &cr, const QString &text,
                                     const QTextCharFormat *format,
                                     bool overwriteMode)
{
    if (cr.hasSelection())
        return replaceSelectedInteractive(cr, text, format);

    if (overwriteMode)
    {
        QTextCursor tmp = cr;
        if (!tmp.atBlockEnd())
        {
            tmp.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            bool ret = replaceSelectedInteractive(tmp, text, format);
            if (ret)
                cr = tmp;
            return ret;
        }
    }

    Block *block = Block::atCursor(cr);
    if (!block || !block->isEditable())
    {
        emit beep();
        return false;
    }

    insertText(cr, text, format);

    check();
    return true;
}

bool TextDocument::deleteInteractive(QTextCursor &at, DeleteType type)
{
    QTextCursor cr = at;

    if (cr.hasSelection())
        type = DeleteSelected;

    if (!cr.hasSelection())
    {
        if (type == DeleteSelected)
            return false;

        switch (type)
        {
            case DeletePreviousChar:
                cr.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                break;
            case DeleteNextChar:
                cr.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                break;
            case DeleteEndOfWord:
                cr.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
                break;
            case DeleteStartOfWord:
                cr.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
                break;
            case DeleteEndOfLine:
                {
                    QTextBlock b = cr.block();
                    if (cr.position() == b.position() + b.length() - 2)
                        cr.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                    else
                        cr.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                }
                break;
            case DeleteSelected:
                break;
        }
    }

    if (!cr.hasSelection())
        return false;

    QTextCursor start = cr, end = cr;
    end.setPosition(cr.anchor());
    if (start.position() > end.position())
        qSwap(start, end);

    Block *sb = Block::atCursor(start);
    Block *eb = Block::atCursor(end);
    m_return_val_if_fail(sb && eb, false);

    if (sb == eb)
    {
        bool ret = false;

        if (!sb->isEditable())
        {
            emit beep();
        }
        else
        {
            deleteText(cr);
            at = cr;
            ret = true;
        }

        check();
        return ret;
    }

    if (type != DeleteSelected)
    {
        emit beep();
        return false;
    }

    if (start != sb->startCursor() || end != eb->startCursor())
    {
        beep();
        return false;
    }

    M_ASSERT(sb != eb);

    while (true)
    {
        Block *next = sb->next();
        deleteBlock(sb);
        if (!next || next == eb)
            break;
        sb = next;
    }

    check();

    at = start;
    return true;
}

bool TextDocument::deleteBlocksInteractive(QTextCursor &cr)
{
    bool deleted = false;
    Block *start, *end;

    if (cr.hasSelection())
    {
        start = Block::atPosition(this, cr.selectionStart());
        end = Block::atPosition(this, cr.selectionEnd());
        if (end && end->startCursor().position() < cr.selectionEnd())
            end = end->next();
    }
    else
    {
        start = Block::atCursor(cr);
        end = start ? start->next() : 0;
    }

    if (!start)
    {
        beep();
        return false;
    }

    for (Block *b = start; b != 0; )
    {
        Block *next = b->next();
        if (next == end)
            next = 0;
        deleteBlock(b);
        deleted = true;
        b = next;
    }

    if (deleted)
    {
        if (end)
            cr = end->startCursor();
        else
            cr = cursor(-1);
    }

    if (!deleted)
        beep();

    return deleted;
}

inline static int countLines(const QString &text)
{
    int n = 1;
    for (int i = 0; i < text.size(); ++i)
    {
        QChar c = text[i];
        if (c == QChar::ParagraphSeparator || c == '\n')
            n++;
        else if (c == '\r')
        {
            n++;
            if (i + 1 < text.size() && text[i+1] == '\n')
                i++;
        }
    }
    return n;
}

void TextDocumentPrivate::insertText(QTextCursor &cr, const QString &text, const QTextCharFormat *format)
{
    Block *block = Block::atCursor(cr);
    m_return_if_fail(block != 0);

    if (text.isEmpty())
        return;

    bool undoable = block->isEditable();
    rt::Fragment fragment(text, format);
    modify(fragment, cr, undoable);

    pub->check();
}

void TextDocumentPrivate::insertFragment(QTextCursor &cr, const rt::Fragment &fragment)
{
    Block *block = Block::atCursor(cr);
    m_return_if_fail(block != 0);

    if (fragment.isEmpty())
        return;

    bool undoable = block->isEditable();
    modify(fragment, cr, undoable);

    pub->check();
}

void TextDocumentPrivate::deleteText(QTextCursor &cr)
{
    if (!cr.hasSelection())
        return;

    Block *block = Block::atCursor(cr);
    m_return_if_fail(block != 0);
    m_return_if_fail(block == Block::atPosition(pub, cr.anchor()));

    modify(cr, block->isEditable());

    pub->check();
}

void TextDocumentPrivate::applyFormat(QTextCursor &cr, const QTextCharFormat &format)
{
    if (!cr.hasSelection())
        return;

    m_implement_me();
    modify(cr, format, true);

    pub->check();
}

void TextDocument::setHrefParser(QString (*parse)(QString const&))
{
    impl->hrefParser = parse;
}

void TextDocument::check()
{
#ifdef ENABLE_DEBUG_CHECK
    QTextCursor cr = cursor();
    Block *block = impl->first;

    if (!block)
    {
        M_ASSERT(!cr.block().userData());
        M_ASSERT(qdoc()->toPlainText().isEmpty());
        return;
    }

    while (true)
    {
        M_ASSERT(block->impl->start <= cr);
        M_ASSERT(cr <= block->impl->end);

        WsUserData *ud = dynamic_cast<WsUserData*>(cr.block().userData());
        M_ASSERT(ud != 0);
        M_ASSERT(ud->block == block);

        if (!cr.movePosition(QTextCursor::NextBlock))
        {
            cr.movePosition(QTextCursor::End);
            M_ASSERT(block->impl->end == cr);
            return;
        }

        if (block->impl->next && block->impl->next->impl->start == cr)
        {
            QTextCursor tmp = cr;
            tmp.movePosition(QTextCursor::PreviousBlock);
            if (!tmp.atBlockEnd())
                tmp.movePosition(QTextCursor::EndOfBlock);
            M_ASSERT(tmp == block->impl->end);
            block = block->impl->next;
        }
    }
#endif
}


void TextDocument::beginAction()
{
    impl->undo_stack.beginGroup();
}

void TextDocument::endAction()
{
    impl->undo_stack.endGroup();
}

bool TextDocument::isRedoAvailable() const
{
    return impl->undo_stack.canRedo();
}

bool TextDocument::isUndoAvailable() const
{
    return impl->undo_stack.canUndo();
}

void TextDocument::undo()
{
    undo(0);
}

void TextDocument::redo()
{
    redo(0);
}

bool TextDocument::isModified() const
{
    return impl->undo_stack.isModified();
}

void TextDocument::setModified(bool modified)
{
    impl->undo_stack.setModified(modified);
}

void TextDocumentPrivate::nonUndoableModification()
{
    undo_stack.setModified(true);
    undo_stack.clear();
}


struct moo::ws::UndoCommand : public QUndoCommand {
    enum CommandId {
        Insert = 1,
        Delete = 2,
        ApplyFormat = 3
    };

    QPointer<TextDocument> doc;

    UndoCommand(TextDocument *doc) : doc(doc) {}
};

class moo::ws::EditCommand : public moo::ws::UndoCommand {
    enum Type {
        Insert = UndoCommand::Insert,
        Delete = UndoCommand::Delete
    };

    Type type;
    int position;
    int length;
    rt::Fragment fragment;

public:
    EditCommand(TextDocument *doc, const rt::Fragment &fragment, const QTextCursor &cr) :
        UndoCommand(doc),
        type(Insert), position(cr.position()),
        length(fragment.length()), fragment(fragment)
    {
        M_ASSERT(!fragment.isEmpty());
    }

    EditCommand(TextDocument *doc, const QTextCursor &cr) :
        UndoCommand(doc), type(Delete)
    {
        position = cr.anchor();
        length = cr.position() - cr.anchor();
        M_ASSERT(length != 0);
    }

    virtual int id() const
    {
        return type;
    }

    virtual bool mergeWith(const QUndoCommand *command)
    {
        const EditCommand *other = static_cast<const EditCommand*>(command);

        switch (type)
        {
            case Insert:
                if (other->length != 1 || other->position != position + length)
                    return false;
                fragment.append(other->fragment);
                length += other->length;
                return true;

            case Delete:
                if (other->length == 1 && length > 0 && other->position == position)
                {
                    fragment.append(other->fragment);
                    length += other->length;
                    return true;
                }
                else if (other->length == -1 && length < 0 && other->position == position + length)
                {
                    fragment.prepend(other->fragment);
                    length += other->length;
                    return true;
                }
                return false;
        }

        return false;
    }

    QTextCharFormat formatForNewText(const QTextCursor &cr, Block *block)
    {
        QTextCharFormat fmt = block->format().charFormat();
        if (block->isEditable() && (!cr.atBlockStart() || !cr.atBlockEnd()))
            fmt.merge(cr.charFormat());
        return fmt;
    }

    QTextCharFormat get_format_for_insert(const QTextCursor &cr)
    {
        Block *b = Block::atCursor(cr);
        m_return_val_if_fail(b != 0, QTextCharFormat());
        if (!cr.atBlockStart() || !cr.atBlockEnd() || cr != b->startCursor())
            return cr.charFormat();
        else
            return b->format().charFormat();
    }

    void insertFragment(QTextCursor &cr, const rt::Fragment &fragment)
    {
        const QTextCharFormat char_format = get_format_for_insert(cr);
        const QTextBlockFormat block_format = cr.blockFormat();

        for (int i = 0; i < fragment.blockCount(); ++i)
        {
            const rt::Block &b = fragment.block(i);
            QTextBlockFormat bf = block_format;
            bf.merge(b.format());

            if (i != 0)
                cr.insertBlock(bf);
            else
                cr.setBlockFormat(bf);

            for (int j = 0; j < b.count(); ++j)
            {
                const rt::Chunk &c = b.chunk(j);
                QTextCharFormat f = char_format;
                if (!c.isPlainText())
                    f.merge(c.format());
                if (f.isAnchor())
                {
                    f.setAnchorHref(doc->impl->parseHref(f.anchorHref()));
                    f.setUnderlineStyle(QTextCharFormat::SingleUnderline);
                    f.setForeground(Qt::blue);
                }
                cr.insertText(c.text(), f);
            }
        }
    }

    void doInsert(bool redo_insert)
    {
        QTextCursor cr = doc->cursor();
        // cursor.setPosition should be enclosed into begin/endEditBlock,
        // otherwise QTextDocument will try to do layout calculations!
        cr.beginEditBlock();

        int pos = length < 0 ? position + length : position;
        cr.setPosition(pos);

        Block *block = Block::atCursor(cr);
        if (!block)
        {
            cr.endEditBlock();
            m_return_if_fail(block != 0);
        }

        bool move_back = false;
        int start_pos = cr.position();
        int start_line = cr.blockNumber();

        if (cr == block->impl->start)
            move_back = true;

        insertFragment(cr, fragment);
        if (!length)
            length = cr.position() - start_pos;

        int end_line = cr.blockNumber();

        if (move_back)
            block->impl->start.setPosition(start_pos);

        if (end_line != start_line)
        {
            QTextCursor tmp = doc->cursor(start_pos);
            tmp.movePosition(QTextCursor::NextBlock);

            for (int i = 0; i < end_line - start_line; ++i)
            {
                tmp.block().setUserData(new WsUserData(block));
                tmp.movePosition(QTextCursor::NextBlock);
            }
        }

        if (doc->impl->undo_cursor)
        {
            if (!redo_insert && length > 0)
                cr.setPosition(position);
            *doc->impl->undo_cursor = cr;
        }

        cr.endEditBlock();
    }

    void redoInsert()
    {
        doInsert(true);
    }

    void undoDelete()
    {
        doInsert(false);
    }

    void doDelete()
    {
        QTextCursor cr = doc->cursor(position);
        cr.setPosition(position + length, QTextCursor::KeepAnchor);

        fragment = rt::Fragment(cr);
        cr.removeSelectedText();

        if (doc->impl->undo_cursor)
            *doc->impl->undo_cursor = cr;
    }

    void redoDelete()
    {
        doDelete();
    }

    void undoInsert()
    {
        doDelete();
    }

    virtual void redo()
    {
        m_return_if_fail(doc);

        switch (type)
        {
            case Insert:
                redoInsert();
                return;
            case Delete:
                redoDelete();
                return;
        }

        m_return_if_reached();
    }

    virtual void undo()
    {
        m_return_if_fail(doc);

        switch (type)
        {
            case Insert:
                undoInsert();
                return;
            case Delete:
                undoDelete();
                return;
        }

        m_return_if_reached();
    }
};

// insert
void TextDocumentPrivate::modify(const QString &text, const QTextCursor &cr, bool undoable)
{
    EditCommand *cmd = new EditCommand(pub, rt::Fragment(text), cr);
    modify(cmd, undoable);
}

void TextDocumentPrivate::modify(const rt::Fragment &fragment, const QTextCursor &cr, bool undoable)
{
    EditCommand *cmd = new EditCommand(pub, fragment, cr);
    modify(cmd, undoable);
}

// delete
void TextDocumentPrivate::modify(const QTextCursor &cr, bool undoable)
{
    EditCommand *cmd = new EditCommand(pub, cr);
    modify(cmd, undoable);
}

namespace {
class ApplyFormatCommand : public UndoCommand {
    int position;
    int length;
    QTextCharFormat fmt;

public:
    ApplyFormatCommand(TextDocument *doc, const QTextCursor &cr, const QTextCharFormat &fmt) :
        UndoCommand(doc), fmt(fmt)
    {
        position = cr.selectionStart();
        length = cr.selectionEnd() - position;
    }

    void redo()
    {
        QTextCursor cr = doc->cursor(position);
        cr.setPosition(position + length, QTextCursor::KeepAnchor);
        cr.mergeCharFormat(fmt);
    }

    void undo()
    {
        qDebug() << Q_FUNC_INFO;
        m_implement_me();
    }

    int id() const
    {
        return UndoCommand::ApplyFormat;
    }
};
}

void TextDocumentPrivate::modify(const QTextCursor &cr, const QTextCharFormat &fmt, bool undoable)
{
    ApplyFormatCommand *cmd = new ApplyFormatCommand(pub, cr, fmt);
    modify(cmd, undoable);
}

void TextDocumentPrivate::modify(UndoCommand *cmd, bool undoable)
{
    if (undoable)
    {
        undo_stack.push(cmd);
    }
    else
    {
        nonUndoableModification();
        cmd->redo();
        delete cmd;
    }
}

void TextDocumentPrivate::undoRedo(bool undo, QTextCursor *cr)
{
    undo_cursor = cr;
    if (undo)
        undo_stack.undo();
    else
        undo_stack.redo();
    undo_cursor = 0;
}

void TextDocument::undo(QTextCursor *cr)
{
    impl->undoRedo(true, cr);
}

void TextDocument::redo(QTextCursor *cr)
{
    impl->undoRedo(false, cr);
}
