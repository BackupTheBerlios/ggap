#include "moows/worksheet-p.h"
#include "moows/colorcombobox.h"
#include "moo-macros.h"

using namespace moo::ws;

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

namespace {

class FormatAction : public QAction {
    Q_OBJECT

protected:
    QPointer<Worksheet> ws;

    FormatAction(Worksheet *ws, QObject *parent) :
        QAction(parent),
        ws(ws)
    {
        connect(ws, SIGNAL(currentCharFormatChanged(bool, const QTextCharFormat&)),
                SLOT(currentCharFormatChanged(bool, const QTextCharFormat&)));
        connect(ws, SIGNAL(currentBlockFormatChanged(bool, const QTextBlockFormat&)),
                SLOT(currentBlockFormatChanged(bool, const QTextBlockFormat&)));
    }

private:
    virtual void updateFromCharFormat(const QTextCharFormat &)
    {
    }

    virtual void updateFromBlockFormat(const QTextBlockFormat &)
    {
    }

private Q_SLOTS:
    void currentCharFormatChanged(bool valid, const QTextCharFormat &fmt)
    {
        setEnabled(valid);
        if (valid)
            updateFromCharFormat(fmt);
    }

    void currentBlockFormatChanged(bool valid, const QTextBlockFormat &fmt)
    {
        setEnabled(valid);
        if (valid)
            updateFromBlockFormat(fmt);
    }
};

class FormatWidgetAction : public QWidgetAction {
    Q_OBJECT

protected:
    QPointer<Worksheet> ws;

    FormatWidgetAction(Worksheet *ws, QObject *parent) :
        QWidgetAction(parent),
        ws(ws)
    {
        connect(ws, SIGNAL(currentCharFormatChanged(bool, const QTextCharFormat&)),
                SLOT(currentCharFormatChanged(bool, const QTextCharFormat&)));
        connect(ws, SIGNAL(currentBlockFormatChanged(bool, const QTextBlockFormat&)),
                SLOT(currentBlockFormatChanged(bool, const QTextBlockFormat&)));
    }

    void updateWidgets()
    {
        QList<QWidget*> widgets = createdWidgets();
        foreach (QWidget *w, widgets)
            updateWidget(w);
    }

private:
    virtual void updateWidget(QWidget *)
    {
    }

    virtual void updateFromCharFormat(const QTextCharFormat &)
    {
    }

    virtual void updateFromBlockFormat(const QTextBlockFormat &)
    {
    }

private Q_SLOTS:
    void currentCharFormatChanged(bool valid, const QTextCharFormat &fmt)
    {
        setEnabled(valid);
        if (valid)
            updateFromCharFormat(fmt);
    }

    void currentBlockFormatChanged(bool valid, const QTextBlockFormat &fmt)
    {
        setEnabled(valid);
        if (valid)
            updateFromBlockFormat(fmt);
    }
};

class FontButtonAction : public FormatWidgetAction {
    Q_OBJECT
    QString family;
    int size;

public:
    FontButtonAction(Worksheet *ws, QObject *parent) :
        FormatWidgetAction(ws, parent)
    {
    }

private:
    QWidget *createWidget(QWidget *parent)
    {
        QWidget *btn = new QPushButton(parent);
        connect(btn, SIGNAL(clicked()), SLOT(buttonClicked()));
        return btn;
    }

    void updateFromCharFormat(const QTextCharFormat &fmt)
    {
        family = fmt.fontFamily();
        size = fmt.fontPointSize();
        if (family.isEmpty())
            family = ws->font().family();
        if (!size)
            size = ws->font().pointSize();
        updateWidgets();
    }

    void updateWidget(QWidget *wid)
    {
        QPushButton *b = dynamic_cast<QPushButton*>(wid);
        m_return_if_fail(b != 0);
        b->setText(QString("%1, %2").arg(family).arg(size));
    }

private Q_SLOTS:
    void buttonClicked()
    {
        QFont font(family, size);
        bool ok;
        font = QFontDialog::getFont(&ok, font, ws, "Choose Font");
        if (!ok)
            return;

        QString newFamily = font.family();
        int newSize = font.pointSize();
        if (family == newFamily && size == newSize)
            return;

        family = newFamily;
        size = newSize;

        QTextCharFormat fmt;
        fmt.setFont(font);
        ws->setCurrentCharFormat(fmt);

        updateWidgets();
    }
};

class ColorAction : public FormatWidgetAction {
    Q_OBJECT
    QColor color;

public:
    ColorAction(Worksheet *ws, QObject *parent) :
        FormatWidgetAction(ws, parent)
    {
    }

private:
    QWidget *createWidget(QWidget *parent)
    {
        ColorComboBox *combo = new ColorComboBox(parent);
        combo->setStandardColors();
        connect(combo, SIGNAL(activated(const QColor&)),
                SLOT(activated(const QColor&)));
        combo->setFocusPolicy(Qt::FocusPolicy(Qt::StrongFocus & ~Qt::ClickFocus));
        return combo;
    }

    void updateFromCharFormat(const QTextCharFormat &fmt)
    {
        QBrush brush = fmt.foreground();
        color = brush.color();
        updateWidgets();
    }

    void updateWidget(QWidget *wid)
    {
        ColorComboBox *combo = dynamic_cast<ColorComboBox*>(wid);
        m_return_if_fail(combo != 0);
        combo->setCurrentColor(color);
    }

private Q_SLOTS:
    void activated(const QColor &newColor)
    {
        if (color == newColor)
            return;

        color = newColor;

        QTextCharFormat fmt;
        fmt.setForeground(QBrush(color));
        ws->setCurrentCharFormat(fmt);

        updateWidgets();
    }
};

class FontNameAction : public FormatWidgetAction {
    Q_OBJECT
    QString family;

public:
    FontNameAction(Worksheet *ws, QObject *parent) :
        FormatWidgetAction(ws, parent)
    {
    }

private:
    QWidget *createWidget(QWidget *parent)
    {
        QFontComboBox *combo = new QFontComboBox(parent);
        connect(combo, SIGNAL(currentFontChanged(const QFont&)),
                SLOT(currentFontChanged(const QFont&)));
        return combo;
    }

    void updateFromCharFormat(const QTextCharFormat &fmt)
    {
        family = fmt.fontFamily();
        if (family.isEmpty())
            family = ws->font().family();
        updateWidgets();
    }

    void updateWidget(QWidget *wid)
    {
        QFontComboBox *fb = dynamic_cast<QFontComboBox*>(wid);
        m_return_if_fail(fb != 0);
        fb->setCurrentFont(QFont(family));
    }

private Q_SLOTS:
    void currentFontChanged(const QFont &newFont)
    {
        if (family == newFont.family())
            return;

        family = newFont.family();

        QTextCharFormat fmt;
        fmt.setFontFamily(family);
        ws->setCurrentCharFormat(fmt);

        updateWidgets();
    }
};

struct FontSizes {
    QList<int> list;

    FontSizes()
    {
        list << 6 << 8;
    }
};

static FontSizes font_sizes;

class FontSizeAction : public FormatWidgetAction {
    Q_OBJECT
    int size;

public:
    FontSizeAction(Worksheet *ws, QObject *parent) :
        FormatWidgetAction(ws, parent)
    {
    }

private:
    QWidget *createWidget(QWidget *parent)
    {
        QComboBox *combo = new QComboBox(parent);
        combo->setEditable(true);

        foreach (int s, font_sizes.list)
            combo->addItem(QString("%1").arg(s));

        QIntValidator *v = new QIntValidator(combo);
        v->setBottom(1);
        v->setTop(1000);
        combo->setValidator(v);

        connect(combo, SIGNAL(currentIndexChanged(const QString&)),
                SLOT(currentIndexChanged(const QString&)));
        return combo;
    }

    void updateFromCharFormat(const QTextCharFormat &fmt)
    {
        size = fmt.fontPointSize();
        if (!size)
            size = ws->font().pointSize();
        updateWidgets();
    }

    void updateWidget(QWidget *wid)
    {
        QComboBox *combo = dynamic_cast<QComboBox*>(wid);
        m_return_if_fail(combo != 0);

        int index = font_sizes.list.indexOf(size);
        if (index >= 0)
            combo->setCurrentIndex(index);
        else
            combo->setEditText(QString("%1").arg(size));
    }

private Q_SLOTS:
    void currentIndexChanged(const QString &text)
    {
        int newSize = text.toInt();

        if (newSize < 1 || newSize > 1000)
            return;

        if (newSize == size)
            return;

        size = newSize;

        QTextCharFormat fmt;
        fmt.setFontPointSize(size);
        ws->setCurrentCharFormat(fmt);

        updateWidgets();
    }
};

enum TextAttribute {
    AttrBold,
    AttrItalic,
    AttrUnderline
};

class TextAttributeAction : public FormatAction {
    Q_OBJECT

    TextAttribute attr;

public:
    TextAttributeAction(Worksheet *ws, TextAttribute attr, QObject *parent) :
        FormatAction(ws, parent),
        attr(attr)
    {
        setCheckable(true);
        connect(this, SIGNAL(triggered(bool)), SLOT(triggered(bool)));

        switch (attr)
        {
            case AttrBold:
                setIcon(QIcon(":/icons32/bold.png"));
                setText("Bold");
                break;
            case AttrItalic:
                setIcon(QIcon(":/icons32/italic.png"));
                setText("Italic");
                break;
            case AttrUnderline:
                setIcon(QIcon(":/icons32/underline.png"));
                setText("Underline");
                break;
        }
    }

private:
    void updateFromCharFormat(const QTextCharFormat &fmt)
    {
        switch (attr)
        {
            case AttrBold:
                setChecked(fmt.fontWeight() == QFont::Bold);
                break;
            case AttrItalic:
                setChecked(fmt.fontItalic());
                break;
            case AttrUnderline:
                setChecked(fmt.fontUnderline());
                break;
        }
    }

private Q_SLOTS:
    void triggered(bool checked)
    {
        QTextCharFormat fmt;

        switch (attr)
        {
            case AttrBold:
                fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
                break;
            case AttrItalic:
                fmt.setFontItalic(checked);
                break;
            case AttrUnderline:
                fmt.setFontUnderline(checked);
                break;
        }

        ws->setCurrentCharFormat(fmt);
    }
};

class AlignmentAction : public FormatAction {
    Q_OBJECT

    Qt::Alignment align;

public:
    AlignmentAction(Worksheet *ws, Qt::Alignment align, QObject *parent) :
        FormatAction(ws, parent),
        align(align)
    {
        setCheckable(true);
        connect(this, SIGNAL(triggered(bool)), SLOT(triggered(bool)));

        switch (align)
        {
            case Qt::AlignLeft:
                setIcon(QIcon(":/icons32/justify-left.png"));
                setText("Align Left");
                break;
            case Qt::AlignRight:
                setIcon(QIcon(":/icons32/justify-right.png"));
                setText("Align Right");
                break;
            case Qt::AlignHCenter:
                setIcon(QIcon(":/icons32/justify-center.png"));
                setText("Center");
                break;
            case Qt::AlignJustify:
                setIcon(QIcon(":/icons32/justify-fill.png"));
                setText("Justify");
                break;
            default:
                break;
        }
    }

private:
    void updateFromBlockFormat(const QTextBlockFormat &fmt)
    {
        setChecked(fmt.alignment() == align);
    }

private Q_SLOTS:
    void triggered(bool checked)
    {
        if (checked)
        {
            QTextBlockFormat fmt;
            fmt.setAlignment(align);
            ws->setCurrentBlockFormat(fmt);
        }
    }
};

class RiseAction : public FormatAction {
    Q_OBJECT

    QTextCharFormat::VerticalAlignment align;

public:
    RiseAction(Worksheet *ws, QTextCharFormat::VerticalAlignment align, QObject *parent) :
        FormatAction(ws, parent),
        align(align)
    {
        setCheckable(true);
        connect(this, SIGNAL(triggered(bool)), SLOT(triggered(bool)));

        switch (align)
        {
            case QTextCharFormat::AlignSuperScript:
                setIconText("^");
                setText("Superscript");
                break;
            case QTextCharFormat::AlignSubScript:
                setIconText("_");
                setText("Subscript");
                break;
            default:
                setVisible(false);
                break;
        }
    }

private:
    void updateFromCharFormat(const QTextCharFormat &fmt)
    {
        setChecked(fmt.verticalAlignment() == align);
    }

private Q_SLOTS:
    void triggered(bool checked)
    {
        if (checked)
        {
            QTextCharFormat fmt;
            fmt.setVerticalAlignment(align);
            ws->setCurrentCharFormat(fmt);
        }
    }

    void maybeTrigger(bool otherChecked)
    {
        if (!otherChecked)
            trigger();
    }
};

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

#include "moc_worksheet-format.cpp"
