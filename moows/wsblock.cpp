#include "moows/wsblock_p.h"
#include "moows/wstextdocument.h"
#include "moo-macros.h"

using namespace moo::ws;

BlockPrivate::BlockPrivate(Block *q) :
    pub(q),
    doc(0), prev(0), next(0),
    start(), end(),
    editable(false),
    format()
{
}

BlockPrivate::~BlockPrivate()
{
}

void Block::setText(const QString &text)
{
    m_return_if_fail(impl->doc != 0);
    QTextCursor cr = startCursor();
    cr.setPosition(endCursor().position(), QTextCursor::KeepAnchor);
    impl->doc->beginAction();
    impl->doc->deleteText(cr);
    impl->doc->insertText(cr, text, 0);
    impl->doc->endAction();
}

void Block::insertText(QTextCursor &cr, const QString &text)
{
    M_ASSERT(startCursor() <= cr && cr <= endCursor());
    impl->doc->insertText(cr, text, 0);
}

void Block::insertFragment(QTextCursor &cr, const rt::Fragment &fragment)
{
    M_ASSERT(startCursor() <= cr && cr <= endCursor());
    impl->doc->insertFragment(cr, fragment);
}

void Block::setFormat(const BlockFormat &fmt)
{
    impl->format = fmt;

    if (!impl->doc)
        return;

    QTextCharFormat char_fmt = fmt.charFormat();
    QTextBlockFormat block_fmt = fmt.blockFormat();
    QTextCursor cr = startCursor();
    cr.setPosition(endCursor().position(), QTextCursor::KeepAnchor);
    cr.mergeCharFormat(char_fmt);
    cr.mergeBlockFormat(block_fmt);
}

Block *Block::atCursor(const QTextCursor &cr)
{
    WsUserData *data = dynamic_cast<WsUserData*>(cr.block().userData());
    return data ? data->block : 0;
}

Block *Block::atPosition(TextDocument *doc, int position)
{
    m_return_val_if_fail(doc != 0, 0);
    return atCursor(doc->cursor(position));
}

Block *Block::fromUserData(QTextBlockUserData *ud)
{
    WsUserData *wd = dynamic_cast<WsUserData*>(ud);
    return wd ? wd->block : 0;
}

Block::Block() :
    QObject(0), impl(this)
{
}

Block::~Block()
{
}

const BlockFormat &Block::format() const
{
    return impl->format;
}

void Block::added()
{
    // do nothing, for subclasses to safely chain up
}

void Block::removed()
{
    // do nothing, for subclasses to safely chain up
}

void Block::setEditable(bool editable)
{
    impl->editable = editable;
}

bool Block::isEditable() const
{
    return impl->editable;
}

QTextCursor Block::startCursor() const
{
    return impl->start;
}

QTextCursor Block::endCursor() const
{
    return impl->end;
}

QTextCursor Block::cursorAtLine(int n) const
{
    if (n < 0)
    {
        QTextCursor cr = impl->end;
        if (!cr.atBlockStart())
            cr.movePosition(QTextCursor::StartOfBlock);
        return cr;
    }
    else
    {
        bool warn = false;
        QTextCursor cr = impl->start;
        if (!cr.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, n))
            warn = true;
        if (cr > impl->end)
        {
            cr = cursorAtLine(-1);
            warn = true;
        }
        if (warn)
            qCritical("%s: invalid value %d", Q_FUNC_INFO, n);
        return cr;
    }
}

TextDocument *Block::document()
{
    return impl->doc;
}

Block *Block::next()
{
    return impl->next;
}

Block *Block::prev()
{
    return impl->prev;
}

const TextDocument *Block::document() const
{
    return impl->doc;
}

const Block *Block::next() const
{
    return impl->next;
}

const Block *Block::prev() const
{
    return impl->prev;
}


// BlockPrivate::BlockPrivate(Block *q) :
//     pub(q),
//     margin(0),
//     prev(0), next(0),
//     doc(0), qlist(0)
// {
// }
//
// BlockPrivate::~BlockPrivate()
// {
// }
//
// void BlockPrivate::setDoc(TextDocument *d, TextList *tl)
// {
//     doc = d;
//     qlist = tl;
//     M_Q(Block);
//     tl->setBlock(q);
//     tl->setMargin(margin);
// }
//
// void BlockPrivate::deleteText()
// {
//     if (!qlist)
//         return;
//
//     if (next)
//     {
//         QTextCursor cr(qlist->item(0));
//         cr.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, qlist->count());
//         cr.removeSelectedText();
//         M_ASSERT(!qlist);
//     }
//     else if (prev)
//     {
//         QTextCursor cr(qlist->item(0));
//         cr.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
//         cr.removeSelectedText();
//         M_ASSERT(qlist && qlist->count() == 1);
//         qlist->remove(cr.block());
//         M_ASSERT(!qlist);
//         cr.movePosition(QTextCursor::PreviousBlock);
//         cr.movePosition(QTextCursor::EndOfBlock);
//         cr.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
//         cr.removeSelectedText();
//     }
//     else
//     {
//         QTextCursor cr(qlist->item(0));
//         cr.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
//         cr.removeSelectedText();
//         M_ASSERT(qlist && qlist->count() == 1);
//         qlist->remove(cr.block());
//         M_ASSERT(!qlist);
//     }
// }
//
//
// Block::Block(BlockPrivate *d) :
//     impl(d ? d : new BlockPrivate(this))
// {
// }
//
// Block::~Block()
// {
//     delete impl;
// }
//
// void Block::added()
// {
// }
//
// void Block::removed()
// {
// }
//
// QPen Block::guidePen() const
// {
//     return QPen();
// }
//
// QTextCursor Block::startCursor() const
// {
//     M_DC(Block);
//     m_return_val_if_fail(d->qlist, QTextCursor(d->doc));
//     return d->qlist->startCursor();
// }
//
// QTextCursor Block::endCursor() const
// {
//     M_DC(Block);
//     m_return_val_if_fail(d->qlist, QTextCursor(d->doc));
//     return d->qlist->endCursor();
// }
//
// QTextBlock Block::firstTextBlock() const
// {
//     M_DC(Block);
//     m_return_val_if_fail(d->qlist, d->doc->begin());
//     return d->qlist->firstTextBlock();
// }
//
// QTextBlock Block::lastTextBlock() const
// {
//     M_DC(Block);
//     m_return_val_if_fail(d->qlist, d->doc->begin());
//     return d->qlist->lastTextBlock();
// }
//
// QTextCursor Block::cursorAtLine(int n) const
// {
//     M_DC(Block);
//     m_return_val_if_fail(d->qlist, QTextCursor(d->doc));
//     return d->qlist->cursorAtLine(n);
// }
//
// Block *Block::blockAtCursor(const QTextCursor &cr)
// {
//     TextList *tl = dynamic_cast<TextList*>(cr.currentList());
//     return tl ? tl->block() : 0;
// }
//
// Block *Block::blockAtTextBlock(const QTextBlock &tb)
// {
//     return blockAtCursor(QTextCursor(tb));
// }
//
// int Block::margin() const
// {
//     M_DC(Block);
//     return d->margin;
// }
//
// void Block::setMargin(int margin)
// {
//     M_D(Block);
//     d->margin = margin;
//     if (d->qlist)
//         d->qlist->setMargin(margin);
// }
//
// TextDocument *Block::document()
// {
//     M_D(Block);
//     return d->doc;
// }
//
// const TextDocument *Block::document() const
// {
//     M_DC(Block);
//     return d->doc;
// }
//
// Block *Block::next()
// {
//     M_D(Block);
//     return d->next;
// }
//
// const Block *Block::next() const
// {
//     M_DC(Block);
//     return d->next;
// }
//
// Block *Block::prev()
// {
//     M_D(Block);
//     return d->prev;
// }
//
// const Block *Block::prev() const
// {
//     M_DC(Block);
//     return d->prev;
// }
