#include "moows/worksheet-p.h"
#include "moows/wstextdocument.h"
#include "moows/wsblock-p.h"
#include "moo-macros.h"
#include <QtGui>

using namespace moo::ws;

Worksheet::Worksheet(QWidget *parent) :
    ParentClass(parent), impl(this)
{
    setDocument(impl->doc.qdoc());
//     QPlainTextDocumentLayout *layout = new QPlainTextDocumentLayout(doc);
//     doc->setDocumentLayout(layout);

    viewport()->setMouseTracking(true);

    connect(&impl->doc, SIGNAL(beep()), SLOT(beep()));
    connect(&impl->doc, SIGNAL(redoAvailable(bool)), SIGNAL(redoAvailable(bool)));
    connect(&impl->doc, SIGNAL(undoAvailable(bool)), SIGNAL(undoAvailable(bool)));

    impl->connect(this, SIGNAL(cursorPositionChanged()), SLOT(cursorPositionChanged()));

    // FIXME
    setAcceptDrops(false);

//     setAcceptRichText(false);

//     QTextImageFormat fmt;
//     fmt.setHeight(100);
//     fmt.setWidth(100);
//     fmt.setName(":/tango/icons/document-new.png");
//     textCursor().insertImage(fmt);
//
//     QWidget *w = new QLineEdit("lalala", viewport());
//     w->move(50, 50);
//     w->resize(50, 50);

//     QTextCursor cursor(document());
//     QTextBlock block = cursor.block();
//     QTextListFormat format;
//     format.setStyle(QTextListFormat::ListDecimal);
// //     format.setIndent(0);
//     QTextList *list = cursor.insertList(format);
//     list->add(block);
}

Worksheet::~Worksheet()
{
}

TextDocument &Worksheet::document()
{
    return impl->doc;
}

const TextDocument &Worksheet::document() const
{
    return impl->doc;
}

Block *Worksheet::firstBlock()
{
    return impl->doc.firstBlock();
}

Block *Worksheet::lastBlock()
{
    return impl->doc.lastBlock();
}

const Block *Worksheet::firstBlock() const
{
    return const_cast<Worksheet*>(this)->firstBlock();
}

const Block *Worksheet::lastBlock() const
{
    return const_cast<Worksheet*>(this)->lastBlock();
}

Block *Worksheet::blockAtCursor()
{
    return impl->blockAtCursor();
}

Block *Worksheet::blockAtCursor(const QTextCursor &cr)
{
    return impl->blockAtCursor(cr);
}

void Worksheet::addPromptBlock(bool afterCursor)
{
    impl->addBlock(true, afterCursor);
}

void Worksheet::addPromptBlockBeforeCursor()
{
    addPromptBlock(false);
}

void Worksheet::addPromptBlockAfterCursor()
{
    addPromptBlock(true);
}

void Worksheet::addTextBlock(bool afterCursor)
{
    impl->addBlock(false, afterCursor);
}

void Worksheet::addTextBlockBeforeCursor()
{
    addTextBlock(false);
}

void Worksheet::addTextBlockAfterCursor()
{
    addTextBlock(true);
}

Block *WorksheetPrivate::addBlock(bool prompt, bool afterCursor)
{
    QTextCursor cr = ws->textCursor();
    Block *old = blockAtCursor(cr);
    Block *after = old;

    if (afterCursor)
    {
        if (PromptBlock *pb = dynamic_cast<PromptBlock*>(old))
        {
            Block *b1 = pb->outputBlock(OutputBlock::Stdout);
            Block *b2 = pb->outputBlock(OutputBlock::Stderr);
            if (b1 && b2)
            {
                if (b1->startCursor() < b2->startCursor())
                    after = b2;
                else
                    after = b1;
            }
            else if (b1)
                after = b1;
            else if (b2)
                after = b2;
        }
    }
    else if (!afterCursor)
    {
        if (OutputBlock *ob = dynamic_cast<OutputBlock*>(old))
        {
            Block *prev = ob->promptBlock();
            if (prev)
                old = prev;
        }

        after = old ? old->prev() : 0;
    }

    Block *new_block;
    if (prompt)
        new_block = new PromptBlock;
    else
        new_block = new TextBlock;
    doc.insertBlock(new_block, after);
    cr = new_block->startCursor();
    ws->setTextCursor(cr);
    ws->ensureCursorVisible();

    return new_block;
}

void Worksheet::deleteBlock()
{
    QTextCursor cr = textCursor();
    document().deleteBlocksInteractive(cr);
    setTextCursor(cr);
    ensureCursorVisible();
    // XXX
    if (document().qdoc()->isEmpty())
        viewport()->update();
}


void WorksheetPrivate::commitInput()
{
    if (!in_input)
    {
        beep();
        return;
    }

    QTextCursor cr = ws->textCursor();
    PromptBlock *block = promptBlockAtCursor(cr);

    if (!block)
    {
        beep();
        return;
    }

    ws->setAcceptingInput(false);

    input = block;
    for (int i = 0; i < 2; ++i)
    {
        output[i] = 0;
        if (OutputBlock *ob = input->outputBlock(i))
            doc.deleteBlock(ob);
    }

//     input->clearErrors();

    ws->processInput(input->lines());
}

void Worksheet::beep()
{
    impl->beep();
}

void WorksheetPrivate::beep()
{
    QApplication::beep();
}

void Worksheet::setAcceptingInput(bool accepting_input)
{
    impl->in_input = accepting_input;
}

void Worksheet::processInput(const QStringList&)
{
    // Nothing
}


void Worksheet::reset()
{
    M_D(Worksheet);

    while (d->doc.firstBlock())
        d->doc.deleteBlock(d->doc.firstBlock());

    d->input = 0;
    d->output[0] = d->output[1] = 0;
    setAcceptingInput(false);
}

bool WorksheetPrivate::isAcceptingInput() const
{
    return in_input;
}

void Worksheet::startInput(const QString &)
{
    M_D(Worksheet);
    m_return_if_fail(!d->in_input);

    Block *block = 0;
    Block *after = 0;

    if (d->output[0] || d->output[1] || d->input)
    {
        Block *current;
        if (d->output[0])
            current = d->output[0];
        else if (d->output[1])
            current = d->output[1];
        else
            current = d->input;

        for (Block *tmp = current->next(); tmp != 0; tmp = tmp->next())
        {
            if (isPromptBlock(tmp))
            {
                block = tmp;
                break;
            }
        }

        after = current->next();
    }

    if (!block && after && isPromptBlock(after) && isOutputBlock(after->next()))
        after = after->next();

    if (!after)
        after = d->doc.lastBlock();

    if (!block)
    {
        block = new PromptBlock;
        d->doc.insertBlock(block, after);
    }

    M_ASSERT(isPromptBlock(block));
    d->input = static_cast<PromptBlock*>(block);
    d->output[0] = d->output[1] = 0;

    QTextCursor cr = block->startCursor();
    setTextCursor(cr);
    ensureCursorVisible();
    setAcceptingInput(true);
}


void Worksheet::continueInput()
{
    M_D(Worksheet);
    m_return_if_fail(!d->in_input);
    m_return_if_fail(d->input);

    QTextCursor cr = textCursor();
    d->input->newLine(cr);
    setTextCursor(cr);
    ensureCursorVisible();
    setAcceptingInput(true);
}

void Worksheet::resumeInput(int line, int column)
{
    M_D(Worksheet);
    m_return_if_fail(!d->in_input);
    m_return_if_fail(d->input);
    setTextCursor(d->input->cursorAt(line, column));
    ensureCursorVisible();
    setAcceptingInput(true);
}


void Worksheet::doWriteOutput(const QString &text, bool is_stdout, bool is_markup)
{
    if (text.isEmpty())
        return;

    int idx = is_stdout ? 0 : 1;

    if (!impl->output[idx])
    {
        if (impl->input)
        {
            impl->output[idx] = impl->input->outputBlock(idx);
            if (!impl->output[idx])
            {
                impl->output[idx] = new OutputBlock(is_stdout);
                impl->doc.insertBlock(impl->output[idx], impl->input);
                impl->input->setOutputBlock(idx, impl->output[idx]);
            }
        }
        else
        {
            impl->output[idx] = new OutputBlock(is_stdout);
            impl->doc.appendBlock(impl->output[idx]);
        }
    }

    OutputBlock *out = impl->output[idx];
    out->output(text, is_markup);

    QTextCursor cr = out->endCursor();
    setTextCursor(cr);
    ensureCursorVisible();
}

void Worksheet::writeOutput(const QString &text)
{
    doWriteOutput(text, true, false);
}

void Worksheet::writeError(const QString &text)
{
    doWriteOutput(text, false, false);
}

void Worksheet::writeMarkup(const QString &text)
{
    doWriteOutput(text, true, true);
}

void Worksheet::highlightError(int line, int firstColumn, int lastColumn, const QString &message)
{
    m_implement_me();
    writeError(message + "\n");
    m_return_if_fail(impl->input != 0);
    impl->input->highlightError(line, firstColumn, lastColumn);
}

void Worksheet::addHistory(const QString &string)
{
    resetHistory();
    if (!string.isEmpty() && (impl->history.isEmpty() || impl->history.at(0) != string))
        impl->history.prepend(string);
    while (impl->history.size() > 500)
        impl->history.pop_back();
}

void Worksheet::resetHistory()
{
    impl->history_ptr = -1;
    impl->history_tmp = QString();
}

void WorksheetPrivate::setHistory(const QStringList &items)
{
    ws->resetHistory();
    history = items;
}

void WorksheetPrivate::history_go(bool forth)
{
    QString new_text;
    PromptBlock *block = dynamic_cast<PromptBlock*>(ws->blockAtCursor());

    if (!block)
        goto beep_and_return;

    if (forth)
    {
        if (history_ptr < 0)
            goto beep_and_return;

        if (history_ptr == 0)
        {
            history_ptr = -1;
            new_text = history_tmp;
        }
        else
        {
            history_ptr -= 1;
            new_text = history.at(history_ptr);
        }
    }
    else
    {
        if (history.isEmpty() || history_ptr == history.size() - 1)
            goto beep_and_return;

        if (history_ptr >= 0)
        {
            history_ptr += 1;
            new_text = history.at(history_ptr);
        }
        else
        {
            history_tmp = block->text();
            history_ptr = 0;
            new_text = history.at(0);
        }
    }

    block->setText(new_text);
    ws->setTextCursor(block->cursorAt(-1, -1));
    ws->ensureCursorVisible();
    return;

beep_and_return:
    beep();
}

void WorksheetPrivate::historyNext()
{
    history_go(true);
}

void WorksheetPrivate::historyPrev()
{
    history_go(false);
}

void Worksheet::activateUrl(const QString &anchor)
{
    QDesktopServices::openUrl(QUrl(anchor));
}
