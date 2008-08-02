#include "moows/worksheet-p.h"
#include "moows/wstextblock.h"
#include "moows/wstextdocument-p.h"
#include "moo-macros.h"
#include <QtGui>

using namespace moo::ws;

inline static void insertInteractive(Worksheet *ws, const QString &text, const QTextCharFormat *fmt, bool overwrite)
{
    QTextCursor cr = ws->textCursor();
    ws->document().insertInteractive(cr, text, fmt, overwrite);
    ws->setTextCursor(cr);
}

inline static void deleteInteractive(Worksheet *ws, TextDocument::DeleteType type)
{
    QTextCursor cr = ws->textCursor();
    ws->document().deleteInteractive(cr, type);
    ws->setTextCursor(cr);
}

bool WorksheetPrivate::cursorMoveKeyEvent(QKeyEvent *ev, bool &handled)
{
    handled = false;

    if (ev == QKeySequence::MoveToNextChar ||
        ev == QKeySequence::MoveToPreviousChar ||
        ev == QKeySequence::MoveToNextWord ||
        ev == QKeySequence::MoveToPreviousWord ||
        ev == QKeySequence::MoveToNextLine ||
        ev == QKeySequence::MoveToPreviousLine ||
        ev == QKeySequence::MoveToNextPage ||
        ev == QKeySequence::MoveToPreviousPage ||
        ev == QKeySequence::MoveToStartOfLine ||
        ev == QKeySequence::MoveToEndOfLine ||
        ev == QKeySequence::MoveToStartOfBlock ||
        ev == QKeySequence::MoveToEndOfBlock ||
        ev == QKeySequence::MoveToStartOfDocument ||
        ev == QKeySequence::MoveToEndOfDocument ||
        ev == QKeySequence::SelectNextChar ||
        ev == QKeySequence::SelectPreviousChar ||
        ev == QKeySequence::SelectNextWord ||
        ev == QKeySequence::SelectPreviousWord ||
        ev == QKeySequence::SelectNextLine ||
        ev == QKeySequence::SelectPreviousLine ||
        ev == QKeySequence::SelectNextPage ||
        ev == QKeySequence::SelectPreviousPage ||
        ev == QKeySequence::SelectStartOfLine ||
        ev == QKeySequence::SelectEndOfLine ||
        ev == QKeySequence::SelectStartOfBlock ||
        ev == QKeySequence::SelectEndOfBlock ||
        ev == QKeySequence::SelectStartOfDocument ||
        ev == QKeySequence::SelectEndOfDocument)
            return true;

    return false;
}

static void ensure_caret(Worksheet *ws)
{
    if (!ws->textCursor().hasSelection())
    {
        Qt::TextInteractionFlags flags = ws->textInteractionFlags();
        ws->setTextInteractionFlags(0);
        ws->setTextInteractionFlags(flags);
    }
}

static void prev_block(QTextCursor &cr)
{
    Block *b = Block::atCursor(cr);
    if (b)
        b = b->prev();
    while (b)
    {
        if (isPromptBlock(b))
            break;
        b = b->prev();
    }
    if (b)
        cr = b->startCursor();
}

static void next_block(QTextCursor &cr)
{
    Block *b = Block::atCursor(cr);
    if (b)
        b = b->next();
    while (b)
    {
        if (isPromptBlock(b))
            break;
        b = b->next();
    }
    if (b)
        cr = b->startCursor();
}

bool WorksheetPrivate::keyPressEvent(QKeyEvent *ev)
{
    ensure_caret(ws);

    if (ev == QKeySequence::SelectAll)
        return false;

    if ((ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down) &&
        ev->modifiers() == Qt::ControlModifier)
    {
        if (ev->key() == Qt::Key_Up)
            historyPrev();
        else
            historyNext();
        ev->accept();
        return true;
    }

    if ((ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down) &&
        ev->modifiers() == Qt::AltModifier)
    {
        QTextCursor cr = ws->textCursor();
        if (ev->key() == Qt::Key_Up)
            prev_block(cr);
        else
            next_block(cr);
        ws->setTextCursor(cr);
        ws->ensureCursorVisible();
        ev->accept();
        return true;
    }

    bool handled;
    if (cursorMoveKeyEvent(ev, handled))
    {
        if (handled)
        {
            ev->accept();
            ws->ensureCursorVisible();
        }
        return handled;
    }

    if (ev == QKeySequence::Copy)
    {
        ws->copy();
        ev->accept();
        return true;
    }
    if (ev == QKeySequence::Cut)
    {
        ws->cut();
        goto done;
    }
    if (ev == QKeySequence::Paste)
    {
        ws->paste();
        goto done;
    }
    if (ev == QKeySequence::Undo)
    {
        ws->undo();
        goto done;
    }
    if (ev == QKeySequence::Redo)
    {
        ws->redo();
        goto done;
    }

    if (ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter)
    {
        QTextCursor cr = ws->textCursor();
        Block *block = blockAtCursor(cr);

        if (ev->modifiers() == 0 && isPromptBlock(block))
        {
            commitInput();
        }
        else if (ev->modifiers() == Qt::AltModifier ||
                 (ev->modifiers() == 0 && isOutputBlock(block)))
        {
            QString anchor = anchorAtCursor();
            if (!anchor.isEmpty())
                ws->activateUrl(anchor);
            else
                beep();
            goto done;
        }
        else if (!(ev->modifiers() & ~Qt::ShiftModifier))
        {
            insertInteractive(ws, "\n", charFormat(), false);
            goto done;
        }
        else
        {
            beep();
        }

        ev->accept();
        return true;
    }

    {
        TextDocument::DeleteType type = (TextDocument::DeleteType) 0;

        if (ev->key() == Qt::Key_Backspace && !(ev->modifiers() & ~Qt::ShiftModifier))
            type = TextDocument::DeletePreviousChar;
        else if (ev == QKeySequence::Delete)
            type = TextDocument::DeleteNextChar;
        else if (ev == QKeySequence::DeleteEndOfWord)
            type = TextDocument::DeleteEndOfWord;
        else if (ev == QKeySequence::DeleteStartOfWord)
            type = TextDocument::DeleteStartOfWord;
        else if (ev == QKeySequence::DeleteEndOfLine)
            type = TextDocument::DeleteEndOfLine;

        if (type)
        {
            deleteInteractive(ws, type);
            goto done;
        }
    }

    {
        QString text = ev->text();
        if (text.isEmpty() || (!text.at(0).isPrint() && text.at(0) != QLatin1Char('\t')))
        {
            ev->ignore();
            return true;
        }
        else
            insertInteractive(ws, text, charFormat(), ws->overwriteMode());
    }

done:
//     cursorOn = true;
//     updateCurrentCharFormat();
    ev->accept();
    ws->ensureCursorVisible();
    return true;
}

void Worksheet::inputMethodEvent(QInputMethodEvent *e)
{
    m_implement_me();
    e->ignore();
}

void Worksheet::dragEnterEvent(QDragEnterEvent *e)
{
    m_implement_me();
    e->ignore();
}

void Worksheet::dragMoveEvent(QDragMoveEvent *e)
{
    m_implement_me();
    e->ignore();
}

void Worksheet::dragLeaveEvent(QDragLeaveEvent *e)
{
    m_implement_me();
    e->ignore();
}

void Worksheet::dropEvent(QDropEvent *e)
{
    m_implement_me();
    e->ignore();
}


void WorksheetPrivate::cut()
{
    copy();
    deleteInteractive(ws, TextDocument::DeleteSelected);
}

void WorksheetPrivate::copy()
{
    QTextCursor cr = ws->textCursor();
    if (cr.hasSelection())
    {
        QString text = cr.selectedText();
        text.replace(QChar(QChar::ParagraphSeparator), "\n");
        QMimeData *data = new QMimeData;
        data->setText(text);
        QApplication::clipboard()->setMimeData(data);
    }
}

void WorksheetPrivate::paste()
{
    const QMimeData *data = QApplication::clipboard()->mimeData();
    if (data)
        insertFromMimeData(data);
}

void WorksheetPrivate::insertFromMimeData(const QMimeData *data)
{
    if (!data)
        return;

    QString text = data->text();

    if (text.isNull())
        return;

    // FIXME: paste rich text
    insertInteractive(ws, text, charFormat(), false);
    ws->ensureCursorVisible();
}

void Worksheet::insertFromMimeData(const QMimeData *source)
{
    impl->insertFromMimeData(source);
}


void Worksheet::cut()
{
    impl->cut();
}

void Worksheet::copy()
{
    impl->copy();
}

void Worksheet::paste()
{
    impl->paste();
}

void Worksheet::deleteSelected()
{
    deleteInteractive(this, TextDocument::DeleteSelected);
}

void Worksheet::undo()
{
    QTextCursor cr = textCursor();
    impl->doc.undo(&cr);
    setTextCursor(cr);
    ensureCursorVisible();
}

void Worksheet::redo()
{
    QTextCursor cr = textCursor();
    impl->doc.redo(&cr);
    setTextCursor(cr);
    ensureCursorVisible();
}

void Worksheet::keyPressEvent(QKeyEvent *ev)
{
    if (!impl->keyPressEvent(ev))
        ParentClass::keyPressEvent(ev);
}

bool WorksheetPrivate::canPaste() const
{
    const QMimeData *md = QApplication::clipboard()->mimeData();
    return md && ws->canInsertFromMimeData(md);
}

QMenu *WorksheetPrivate::createContextMenu(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(ws);
    QAction *a;
    QTextCursor cr = ws->textCursor();

#define ACCEL_KEY(k) QString::fromLatin1("\t") + QString(QKeySequence(k))

    QString text = tr("&Undo");
    a = menu->addAction(tr("&Undo") + ACCEL_KEY(QKeySequence::Undo), ws, SLOT(undo()));
    a->setEnabled(doc.isUndoAvailable());
    a = menu->addAction(tr("&Redo") + ACCEL_KEY(QKeySequence::Redo), ws, SLOT(redo()));
    a->setEnabled(doc.isRedoAvailable());
    menu->addSeparator();

    a = menu->addAction(tr("Cu&t") + ACCEL_KEY(QKeySequence::Cut), ws, SLOT(cut()));
    a->setEnabled(cr.hasSelection());

    a = menu->addAction(tr("&Copy") + ACCEL_KEY(QKeySequence::Copy), ws, SLOT(copy()));
    a->setEnabled(cr.hasSelection());

    a = menu->addAction(tr("&Paste") + ACCEL_KEY(QKeySequence::Paste), ws, SLOT(paste()));
    a->setEnabled(canPaste());
    a = menu->addAction(tr("Delete"), ws, SLOT(deleteSelected()));
    a->setEnabled(cr.hasSelection());

    menu->addSeparator();
    a = menu->addAction(tr("Select All") + ACCEL_KEY(QKeySequence::SelectAll), ws, SLOT(selectAll()));
    a->setEnabled(!doc.qdoc()->isEmpty());

    return menu;
}

void Worksheet::contextMenuEvent(QContextMenuEvent *event)
{
    if (!hasFocus())
        return;

    QMenu *menu = impl->createContextMenu(event);
    if (menu)
        menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

void Worksheet::mousePressEvent(QMouseEvent *event)
{
    QTextEdit::mousePressEvent(event);
    const QString anchor = anchorAt(event->pos());
    impl->highlighted(anchor, true);
}

void Worksheet::mouseReleaseEvent(QMouseEvent *event)
{
    QTextEdit::mouseReleaseEvent(event);
    const QString anchor = anchorAt(event->pos());
    if (!anchor.isEmpty())
        activateUrl(anchor);
    impl->highlighted(anchor, true);
}

void Worksheet::mouseMoveEvent(QMouseEvent *event)
{
    QTextEdit::mouseReleaseEvent(event);
    const QString anchor = anchorAt(event->pos());
    impl->highlighted(anchor, true);
}

void Worksheet::leaveEvent(QEvent *event)
{
    QTextEdit::leaveEvent(event);
    impl->highlighted(QString(), true);
}
