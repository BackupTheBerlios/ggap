#include "moows/wstextblock.h"
#include "moows/wstextdocument.h"
#include "moows/worksheet-xml.h"
#include "moo-macros.h"
#include <QtGui>

using namespace moo::ws;


TextBlockBase::TextBlockBase(bool editable) : Block()
{
    setEditable(editable);
}

TextBlockBase::~TextBlockBase()
{
}

void TextBlockBase::added()
{
    Block::added();
}

void TextBlockBase::removed()
{
    Block::removed();
}

void TextBlockBase::setText(const QString &newText)
{
    m_return_if_fail(document() != 0);
    Block::setText(newText);
}

void TextBlockBase::append(const QString &text)
{
    m_return_if_fail(document() != 0);
    QTextCursor cr = endCursor();
    Block::insertText(cr, text);
}

void TextBlockBase::append(const rt::Fragment &fragment)
{
    m_return_if_fail(document() != 0);
    QTextCursor cr = endCursor();
    Block::insertFragment(cr, fragment);
}

QString TextBlockBase::text() const
{
    m_return_val_if_fail(document() != 0, QString());
    QTextCursor cr = startCursor();
    QTextCursor end = endCursor();
    cr.setPosition(end.position(), QTextCursor::KeepAnchor);
    return cr.selectedText();
}

QString TextBlockBase::html() const
{
    m_return_val_if_fail(document() != 0, QString());
    QTextCursor cr = startCursor();
    QTextCursor end = endCursor();
    cr.setPosition(end.position(), QTextCursor::KeepAnchor);
    QTextDocumentFragment fr = cr.selection();;
    return fr.toHtml("UTF-8");
}

QStringList TextBlockBase::lines() const
{
    QTextCursor cr = const_cast<TextBlockBase*>(this)->startCursor();
    QTextCursor end = const_cast<TextBlockBase*>(this)->endCursor();
    cr.setPosition(end.position(), QTextCursor::KeepAnchor);
    QString text = cr.selectedText();
    return text.split(QChar(QChar::ParagraphSeparator));
}


QTextCursor TextBlockBase::cursorAt(int line, int column)
{
    QTextCursor cr = cursorAtLine(line);
    QTextBlock tb = cr.block();
    if (column < 0)
    {
        cr.movePosition(QTextCursor::EndOfBlock);
    }
    else
    {
        cr.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, column);
        if (cr.block() != tb)
        {
            cr = QTextCursor(tb);
            cr.movePosition(QTextCursor::EndOfBlock);
        }
    }
    return cr;
}

void TextBlockBase::newLine(QTextCursor &cr)
{
    if (Block::atCursor(cr) != this)
        cr = endCursor();
    insertText(cr, "\n");
}


struct moo::ws::TextBlockPrivate {
    TextBlockPrivate(TextBlock *b) :
        pub(b)
    {
    }

    M_DECLARE_PUBLIC(TextBlock)
};

TextBlock::TextBlock() :
    TextBlockBase(true), impl(this)
{
    BlockFormat fmt;
    fmt.setForeground(Qt::darkBlue);
    setFormat(fmt);
}

TextBlock::~TextBlock()
{
}


struct moo::ws::OutputBlockPrivate {
    M_DECLARE_PUBLIC(OutputBlock)

    QPointer<PromptBlock> prompt;
    OutputBlock::Type type;
    int newlines;

    OutputBlockPrivate(OutputBlock *b, bool is_stdout) :
        pub(b),
        prompt(0),
        type(is_stdout ? OutputBlock::Stdout : OutputBlock::Stderr),
        newlines(0)
    {
    }
};

OutputBlock::OutputBlock(bool is_stdout) :
    TextBlockBase(false),
    impl(this, is_stdout)
{
    Qt::GlobalColor color;
    if (is_stdout)
        color = Qt::darkGreen;
    else
        color = Qt::red;
    BlockFormat fmt;
    fmt.setForeground(color);
    setFormat(fmt);
}

OutputBlock::~OutputBlock()
{
}

OutputBlock::Type OutputBlock::type() const
{
    M_CD(OutputBlock);
    return d->type;
}

PromptBlock *OutputBlock::promptBlock()
{
    M_D(OutputBlock);
    return d->prompt;
}

void OutputBlock::setPromptBlock(PromptBlock *pb)
{
    M_D(OutputBlock);
    d->prompt = pb;
}

static QStringList split_lines(const QString &text)
{
    // XXX
    return text.split("\n");
}

static void flushNewlines(OutputBlock *ob, int &newlines)
{
    while (newlines)
    {
        ob->append("\n");
        newlines--;
    }
}

void OutputBlock::output(const QString &text, bool markup)
{
    if (markup)
    {
        const rt::Fragment fragment = rt::Fragment::fromMarkup(text);

        for (int i = 0; i < fragment.blockCount(); ++i)
        {
            const rt::Block &block = fragment.block(i);

            if (!block.isEmpty())
                flushNewlines(this, impl->newlines);

            if (i != 0)
            {
                if (block.isEmpty())
                    impl->newlines += 1;
                else
                    append("\n");
            }

            if (!block.isEmpty())
                append(rt::Fragment(block));
        }
    }
    else
    {
        const QStringList lines = split_lines(text);

        for (int i = 0; i < lines.size(); ++i)
        {
            if (!lines.at(i).isEmpty())
                flushNewlines(this, impl->newlines);

            if (i != 0)
            {
                if (lines.at(i).isEmpty())
                    impl->newlines += 1;
                else
                    append("\n");
            }

            if (!lines.at(i).isEmpty())
                append(lines.at(i));
        }
    }
}


struct moo::ws::PromptBlockPrivate {
    M_DECLARE_PUBLIC(PromptBlock)

    QPointer<OutputBlock> output[2];
    bool errors_highlighted;

    PromptBlockPrivate(PromptBlock *b) :
        pub(b), errors_highlighted(false)
    {
    }

    void setOutput(int type, OutputBlock *ob)
    {
        if (output[type] != ob)
        {
            if (output[type])
                output[type]->setPromptBlock(0);
            output[type] = ob;
            if (output[type])
                output[type]->setPromptBlock(pub);
        }
    }
};

PromptBlock::PromptBlock() :
    TextBlockBase(true),
    impl(this)
{
    BlockFormat fmt;
    fmt.setForeground(Qt::darkRed);
    setFormat(fmt);
}

PromptBlock::~PromptBlock()
{
}

OutputBlock *PromptBlock::outputBlock(int type)
{
    m_return_val_if_fail(type >= 0 && type < 2, 0);
    M_D(PromptBlock);
    return d->output[type];
}

void PromptBlock::setOutputBlock(int type, OutputBlock *ob)
{
    m_return_if_fail(type >= 0 && type < 2);
    M_D(PromptBlock);
    d->setOutput(type, ob);
}

void PromptBlock::highlightError(int line, int start_column, int end_column)
{
    QTextCursor cr = cursorAtLine(line);
    cr.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, start_column);
    cr.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, end_column - start_column + 1);

    QTextCharFormat fmt;
    fmt.setUnderlineColor(Qt::red);
    fmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    cr.mergeCharFormat(fmt);

    M_D(PromptBlock);
    d->errors_highlighted = true;
    connect(document(), SIGNAL(contentsChanged()), SLOT(docContentsChanged()));
}

void PromptBlock::docContentsChanged()
{
    document()->disconnect(this);
    clearErrors();
}

void PromptBlock::clearErrors()
{
    M_D(PromptBlock);
    if (d->errors_highlighted)
    {
        QTextCursor cr = startCursor();
        cr.setPosition(endCursor().position(), QTextCursor::KeepAnchor);

        QTextCharFormat fmt;
        fmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
        cr.mergeCharFormat(fmt);

        d->errors_highlighted = false;
    }
}
