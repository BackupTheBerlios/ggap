#include "moows/worksheet-format-p.h"

using namespace moo::ws;
using namespace moo::ws::ui_impl;

static bool currentCharFormat(Worksheet *ws, QTextCharFormat &fmt, const QTextCharFormat *custom_fmt)
{
    Block *b;
    QTextCursor cr = ws->textCursor();

    if (cr.hasSelection())
        // XXX
        b = Block::atPosition(&ws->document(), cr.selectionStart());
    else
        b = Block::atCursor(cr);

    if (!b || !b->isEditable())
    {
        return false;
    }

    if (cr.hasSelection())
    {
        // XXX
        QTextCursor tmp = cr;
        tmp.setPosition(cr.selectionStart());
        tmp.movePosition(QTextCursor::NextCharacter);
        fmt = tmp.charFormat();
    }
    else
    {
        if (!cr.atBlockStart() || !cr.atBlockEnd())
            fmt = cr.charFormat();
        else
            fmt = b->format().charFormat();
        if (custom_fmt)
            fmt.merge(*custom_fmt);
    }

    return true;
}

static bool currentBlockFormat(Worksheet *ws, QTextBlockFormat &fmt)
{
    Block *b;
    QTextCursor cr = ws->textCursor();

    // XXX

    if (cr.hasSelection())
        b = Block::atPosition(&ws->document(), cr.selectionStart());
    else
        b = Block::atCursor(cr);

    if (!b || !b->isEditable())
        return false;

    fmt = cr.blockFormat();
    return true;
}

void WorksheetPrivate::emitFormatChanged()
{
    format_timer_started = false;

    QTextCharFormat char_fmt;
    bool valid = ::currentCharFormat(ws, char_fmt, charFormat());
    emit ws->currentCharFormatChanged(valid, char_fmt);

    QTextBlockFormat block_fmt;
    valid = ::currentBlockFormat(ws, block_fmt);
    emit ws->currentBlockFormatChanged(valid, block_fmt);
}

void WorksheetPrivate::queueFormatChanged()
{
    if (!format_timer_started)
    {
        format_timer_started = true;
        QTimer::singleShot(0, this, SLOT(emitFormatChanged()));
    }
}

void WorksheetPrivate::cursorPositionChanged()
{
    char_fmt_valid = false;
    if (track_format)
        queueFormatChanged();
    highlighted(anchorAtCursor(), false);
}

void Worksheet::emitCursorPositionChanged()
{
    emit QTextEdit::cursorPositionChanged();
}

void Worksheet::setTrackCurrentFormat(bool track)
{
    qDebug() << Q_FUNC_INFO << track;
    if (impl->track_format != track)
    {
        if ((impl->track_format = track))
            emitCursorPositionChanged();
    }
}

void Worksheet::setCurrentCharFormat(const QTextCharFormat &fmt)
{
    impl->setCurrentCharFormat(fmt);
}

void WorksheetPrivate::setCurrentCharFormat(const QTextCharFormat &fmt)
{
    QTextCursor cr = ws->textCursor();

    if (cr.hasSelection())
    {
        // XXX check editable
        doc.applyFormat(cr, fmt);
        char_fmt_valid = false;
    }
    else if (!char_fmt_valid)
    {
        char_fmt_valid = true;
        char_fmt = fmt;
    }
    else
    {
        char_fmt.merge(fmt);
    }
}

void Worksheet::setCurrentBlockFormat(const QTextBlockFormat &fmt)
{
    impl->setCurrentBlockFormat(fmt);
}

void WorksheetPrivate::setCurrentBlockFormat(const QTextBlockFormat &fmt)
{
    QTextCursor cr = ws->textCursor();
    cr.mergeBlockFormat(fmt);
}

// void WorksheetPrivate::setCurrentCharFormat(const QTextCharFormat &format);
// QTextCharFormat WorksheetPrivate::currentCharFormat() const;

QAction *Worksheet::actionFontButton(QObject *parent)
{
    return new FontButtonAction(this, parent);
}

QAction *Worksheet::actionFontName(QObject *parent)
{
    return new FontNameAction(this, parent);
}

QAction *Worksheet::actionFontSize(QObject *parent)
{
    return new FontSizeAction(this, parent);
}

QAction *Worksheet::actionBold(QObject *parent)
{
    return new TextAttributeAction(this, AttrBold, parent);
}

QAction *Worksheet::actionItalic(QObject *parent)
{
    return new TextAttributeAction(this, AttrItalic, parent);
}

QAction *Worksheet::actionUnderline(QObject *parent)
{
    return new TextAttributeAction(this, AttrUnderline, parent);
}

QActionGroup *Worksheet::actionGroupJustify(QObject *parent)
{
    QActionGroup *group = new QActionGroup(parent);
    group->setExclusive(true);
    new AlignmentAction(this, Qt::AlignLeft, group);
    new AlignmentAction(this, Qt::AlignHCenter, group);
    new AlignmentAction(this, Qt::AlignRight, group);
    new AlignmentAction(this, Qt::AlignJustify, group);
    return group;
}

QActionGroup *Worksheet::actionGroupSupSubScript(QObject *parent)
{
    QActionGroup *group = new QActionGroup(parent);
    group->setExclusive(false);
    QAction *na = new RiseAction(this, QTextCharFormat::AlignNormal, group);
    QAction *a = new RiseAction(this, QTextCharFormat::AlignSuperScript, group);
    na->connect(a, SIGNAL(triggered(bool)), SLOT(maybeTrigger(bool)));
    a = new RiseAction(this, QTextCharFormat::AlignSubScript, group);
    na->connect(a, SIGNAL(triggered(bool)), SLOT(maybeTrigger(bool)));
    return group;
}

QAction *Worksheet::actionTextColor(QObject *parent)
{
    return new ColorAction(this, parent);
}
