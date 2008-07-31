#include "moows/worksheet_p.h"
#include "moows/wstextblock.h"
#include "moo-macros.h"
#include <QtGui>

using namespace moo::ws;

static void paintBlockRectangle(Worksheet *ws, const Block *block, QPainter &p)
{
    QTextCursor start_cr = block->startCursor();
    QTextCursor end_cr = block->endCursor();
    QRect r = ws->cursorRect(start_cr);
    r = r.united(ws->cursorRect(end_cr));
    r = QRect(QPoint(1, r.y() + 1), QSize(3, r.height() - 4));
    block->format().applyTo(p);
    p.drawRect(r);
}

static void paintBlockOutline(Worksheet *ws, const Block *block, QPainter &p)
{
    QTextCursor start_cr = block->startCursor();
    QTextCursor end_cr = block->endCursor();
    QRect r = ws->cursorRect(start_cr);
    r = r.united(ws->cursorRect(end_cr));
    r = QRect(QPoint(2, r.y() + 1), QSize(5, r.height() - 4));
    block->format().applyTo(p);
    p.drawLine(r.topRight(), r.topLeft());
    p.drawLine(r.topLeft(), r.bottomLeft());
    p.drawLine(r.bottomLeft(), r.bottomRight());
}

static void paintPromptBlockGuide(Worksheet *ws, const PromptBlock *block, QPainter &p)
{
    const QTextBlock sb = block->startCursor().block();
    const QTextBlock eb = block->endCursor().block();

    block->format().applyTo(p);

    for (QTextBlock b = sb; ; b = b.next())
    {
        const QTextCursor cr(b);
        QRect r = ws->cursorRect(cr);

        QFontMetrics fm(ws->font());
        QRect fr = fm.boundingRect('2');

        int width = 10;
        int height = fr.height();
        int y = r.y() + fm.ascent() - height;
        int k = qMax(1, qMin(width - 3, (height - 1)/2));

        QPoint p1((width - k)/2, y + 1 + (height - 2*k - 1)/2);
        QPoint p2(p1.x() + k, p1.y() + k);
        QPoint p3(p1.x(), p1.y() + 2*k);
        p.drawLine(p1, p2);
        p.drawLine(p2, p3);

        p1.rx() += 1;
        p2.rx() += 1;
        p3.rx() += 1;
        p.drawLine(p1, p2);
        p.drawLine(p2, p3);

        break;

        if (b == eb)
            break;
    }
}

static void paintOutputBlockGuide(Worksheet *ws, const OutputBlock *block, QPainter &p)
{
    paintBlockOutline(ws, block, p);
//     paintBlockRectangle(ws, block, p);
}

static void paintTextBlockGuide(Worksheet *ws, const TextBlock *block, QPainter &p)
{
    paintBlockOutline(ws, block, p);
//     paintBlockRectangle(ws, block, p);
}

static void paintBlockGuide(Worksheet *ws, const Block *block, QPainter &p)
{
    if (const PromptBlock *pb = dynamic_cast<const PromptBlock*>(block))
        paintPromptBlockGuide(ws, pb, p);
    else if (const OutputBlock *ob = dynamic_cast<const OutputBlock*>(block))
        paintOutputBlockGuide(ws, ob, p);
    else if (const TextBlock *tb = dynamic_cast<const TextBlock*>(block))
        paintTextBlockGuide(ws, tb, p);
    else
        m_implement_me();
}

void Worksheet::paintEvent(QPaintEvent *e)
{
    ParentClass::paintEvent(e);

    QPainter p(viewport());
    p.translate(-impl->horizontalOffset(), 0);

    QTextCursor cr = cursorForPosition(e->rect().topLeft());
    QTextCursor last = cursorForPosition(e->rect().bottomLeft());

    const Block *b = Block::atCursor(cr);
    if (!b)
        return;

    do
    {
        p.save();
        paintBlockGuide(this, b, p);
        p.restore();
        b = b->next();
    }
    while (b && !(last < b->startCursor()));
}

void Worksheet::resizeEvent(QResizeEvent *e)
{
    ParentClass::resizeEvent(e);
}
