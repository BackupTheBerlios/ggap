#include "moows/wstextblock.h"
#include "moows/worksheet.h"
#include "moows/worksheet-xml-p.h"
#include "moo-macros.h"
#include <QXmlStreamWriter>
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>
#include <QTextFragment>
#include <QDebug>

using namespace moo::ws;

rt::Fragment::Fragment(const QTextCursor &cr) :
    d(new FragmentData(cr.selectedText()))
{
    M_IMPLEMENT_ME();
}

rt::Fragment::Fragment() :
    d(new FragmentData)
{
}

rt::Fragment::Fragment(const QString &text, const QTextCharFormat *format) :
    d(new FragmentData(text, format))
{
}

rt::Fragment::Fragment(const QList<Block> &blocks) :
    d(new FragmentData(blocks))
{
}

rt::Fragment::Fragment(const Block &block) :
    d(0)
{
    QList<Block> list;
    list << block;
    d = new FragmentData(list);
}

rt::Fragment::~Fragment()
{
}

rt::Fragment::Fragment(const Fragment &fragment) :
    d(fragment.d)
{
}

rt::Fragment &rt::Fragment::operator = (const Fragment &fragment)
{
    d = fragment.d;
    return *this;
}

void rt::Fragment::append(const Fragment &other)
{
    d->append(*other.d);
}

void rt::Fragment::prepend(const Fragment &other)
{
    d->prepend(*other.d);
}

bool rt::Fragment::isPlainText() const
{
    return d->isPlainText();
}

int rt::Fragment::blockCount() const
{
    return d->blockCount();
}

const rt::Block &rt::Fragment::block(int n) const
{
    return d->block(n);
}

int rt::Fragment::length() const
{
    return d->length();
}

bool rt::Fragment::isEmpty() const
{
    return d->isEmpty();
}


namespace {

class XmlWriter {
    TextBlockBase *source;
    QXmlStreamWriter &xml;
    const QTextCharFormat &default_format;

    void writeFragment(const QTextFragment &fragment);
    void writeStartParagraph(const QTextCursor &cr);
    QStringList getCharFormatAttributes(const QTextCharFormat &format);

public:
    XmlWriter(TextBlockBase *text_block, const QTextCharFormat &default_format, QXmlStreamWriter &xml) :
        source(text_block), xml(xml), default_format(default_format)
    {
    }

    void writeBlock();
};

}

QStringList XmlWriter::getCharFormatAttributes(const QTextCharFormat &format)
{
    QStringList attrs;

    const QString family = format.fontFamily();
    if (!family.isEmpty() && family != default_format.fontFamily())
        attrs << "font-family" << family;

    if (format.hasProperty(QTextFormat::FontPointSize) &&
        format.fontPointSize() != default_format.fontPointSize())
    {
        attrs << "font-size" << (QString::number(format.fontPointSize()) + "pt");
    }
    else if (format.hasProperty(QTextFormat::FontSizeAdjustment))
    {
        static const char * const sizes[] = {
            "small", "medium", "large", "x-large", "xx-large"
        };
        int k = format.intProperty(QTextFormat::FontSizeAdjustment) + 1;
        if (k >= 0 && k < 5)
            attrs << "font-size" << sizes[k];
    }

    if (format.hasProperty(QTextFormat::FontWeight) &&
        format.fontWeight() != default_format.fontWeight())
            attrs << "font-weight" << QString::number(format.fontWeight() * 8);

    if (format.hasProperty(QTextFormat::FontItalic) &&
        format.fontItalic() != default_format.fontItalic())
            attrs << "font-style"
                  << (format.fontItalic() ? "italic" : "normal");

    if ((format.hasProperty(QTextFormat::FontUnderline) ||
         format.hasProperty(QTextFormat::TextUnderlineStyle)) &&
        format.fontUnderline() != default_format.fontUnderline() &&
        format.fontUnderline())
            attrs << "underline" << "1";

    if (format.hasProperty(QTextFormat::FontOverline) &&
        format.fontOverline() != default_format.fontOverline())
            attrs << "overline" << "1";

    if (format.hasProperty(QTextFormat::FontStrikeOut) &&
        format.fontStrikeOut() != default_format.fontStrikeOut())
        attrs << "line-through" << "1";

    if (format.foreground() != default_format.foreground() &&
        format.foreground().style() != Qt::NoBrush)
            attrs << "color" << format.foreground().color().name();

    if (format.background() != default_format.background() &&
        format.background().style() == Qt::SolidPattern)
            attrs << "background-color" << format.background().color().name();

    if (format.verticalAlignment() != default_format.verticalAlignment() &&
        format.verticalAlignment() != QTextCharFormat::AlignNormal)
    {
        attrs << "vertical-align";
        QTextCharFormat::VerticalAlignment valign = format.verticalAlignment();
        if (valign == QTextCharFormat::AlignSubScript)
            attrs << "sub";
        else if (valign == QTextCharFormat::AlignSuperScript)
            attrs << "super";
        else if (valign == QTextCharFormat::AlignMiddle)
            attrs << "middle";
        else if (valign == QTextCharFormat::AlignTop)
            attrs << "top";
        else if (valign == QTextCharFormat::AlignBottom)
            attrs << "bottom";
        else
            attrs << "none";
    }

#if QT_VERSION >= 0x040400
    if (format.fontWordSpacing())
        attrs << "word-spacing" << (QString::number(format.fontWordSpacing()) + "px");
#endif

    return attrs;
}

void XmlWriter::writeFragment(const QTextFragment &fragment)
{
    const QTextCharFormat format = fragment.charFormat();
    QStringList attributes = getCharFormatAttributes(format);
    bool elm_started = false;

    if (!attributes.isEmpty() || format.isAnchor())
    {
        elm_started = true;
        if (format.isAnchor())
        {
            xml.writeStartElement("a");
            xml.writeAttribute("href", format.anchorHref());
        }
        else
        {
            xml.writeStartElement("span");
        }
        for (int i = 0; i < attributes.size(); i += 2)
            xml.writeAttribute(attributes.at(i), attributes.at(i+1));
    }

    xml.writeCharacters(fragment.text());

    if (elm_started)
        xml.writeEndElement();
}

static void writeAlignment(QXmlStreamWriter &xml, Qt::Alignment align)
{
    if (align & Qt::AlignRight)
        xml.writeAttribute("align", "right");
    else if (align & Qt::AlignHCenter)
        xml.writeAttribute("align", "center");
    else if (align & Qt::AlignJustify)
        xml.writeAttribute("align", "justify");
}

void XmlWriter::writeStartParagraph(const QTextCursor &cr)
{
    const QTextBlockFormat format = cr.blockFormat();

    xml.writeStartElement("p");

    if (format.hasProperty(QTextFormat::BlockAlignment))
        writeAlignment(xml, format.alignment());
}

void XmlWriter::writeBlock()
{
    QTextCursor cr = source->startCursor();

    do
    {
        writeStartParagraph(cr);
        xml.setAutoFormatting(false);

        const QTextBlock tb = cr.block();
        for (QTextBlock::iterator iter = tb.begin(); !iter.atEnd(); ++iter)
        {
            const QTextFragment fragment = iter.fragment();
            writeFragment(fragment);
        }

        xml.writeEndElement(); // </p>
        xml.setAutoFormatting(true);

        if (!cr.movePosition(QTextCursor::NextBlock))
            break;
    }
    while (cr <= source->endCursor());
}

void Worksheet::formatBlock(QXmlStreamWriter &xml, TextBlockBase *block)
{
    const QTextCharFormat format = block->format().charFormat();
    XmlWriter w(block, format, xml);
    w.writeBlock();
}


template<typename T>
static bool getFormatFromAttributes(const QString &tagName, const T &elm, QTextCharFormat &format)
{
    bool has_format = false;

    if (tagName == "a")
    {
        format.setAnchor(true);
        format.setAnchorHref(elm.attribute("href"));
        has_format = true;
    }
    else if (tagName != "span")
    {
        qWarning() << "unknown tag" << tagName;
    }

    QString a = elm.attribute("font-family");
    if (!a.isEmpty())
    {
        format.setFontFamily(a);
        has_format = true;
    }

    a = elm.attribute("font-size");
    if (!a.isEmpty())
    {
        if (a.endsWith("pt"))
        {
            bool ok = false;
            qreal size = a.mid(0, a.size() - 2).toDouble(&ok);
            if (ok)
                format.setFontPointSize(size);
            else
                qCritical() << Q_FUNC_INFO << "invalid value" << a;
        }
        else if (a == "small")
            format.setProperty(QTextFormat::FontSizeAdjustment, -1);
        else if (a == "medium")
            format.setProperty(QTextFormat::FontSizeAdjustment, 0);
        else if (a == "large")
            format.setProperty(QTextFormat::FontSizeAdjustment, 1);
        else if (a == "x-large")
            format.setProperty(QTextFormat::FontSizeAdjustment, 2);
        else if (a == "xx-large")
            format.setProperty(QTextFormat::FontSizeAdjustment, 3);
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }

    a = elm.attribute("font-weight");
    if (!a.isEmpty())
    {
        bool ok = false;
        int weight = a.toInt(&ok);
        if (ok)
            format.setFontWeight(weight / 8);
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }

    a = elm.attribute("font-style");
    if (!a.isEmpty())
    {
        if (a == "italic")
            format.setFontItalic(true);
        else if (a == "normal")
            format.setFontItalic(false);
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }

    a = elm.attribute("underline");
    if (!a.isEmpty())
    {
        if (a == "1")
            format.setFontUnderline(true);
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }

    a = elm.attribute("overline");
    if (!a.isEmpty())
    {
        if (a == "1")
            format.setFontOverline(true);
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }

    a = elm.attribute("line-through");
    if (!a.isEmpty())
    {
        if (a == "1")
            format.setFontStrikeOut(true);
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }

    a = elm.attribute("color");
    if (!a.isEmpty())
    {
        format.setForeground(QBrush(QColor(a)));
        has_format = true;
    }

    a = elm.attribute("background-color");
    if (!a.isEmpty())
    {
        format.setBackground(QBrush(QColor(a)));
        has_format = true;
    }

    a = elm.attribute("vertical-align");
    if (!a.isEmpty())
    {
        if (a == "sub")
            format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
        else if (a == "super")
            format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        else if (a == "middle")
            format.setVerticalAlignment(QTextCharFormat::AlignMiddle);
        else if (a == "top")
            format.setVerticalAlignment(QTextCharFormat::AlignTop);
        else if (a == "bottom")
            format.setVerticalAlignment(QTextCharFormat::AlignBottom);
        else if (a == "none")
            format.setVerticalAlignment(QTextCharFormat::AlignNormal);
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }

#if QT_VERSION >= 0x040400
    a = elm.attribute("word-spacing");
    if (!a.isEmpty())
    {
        if (a.endsWith("px"))
        {
            bool ok = false;
            qreal spacing = a.mid(0, a.size() - 2).toDouble(&ok);
            if (ok)
                format.setFontWordSpacing(spacing);
            else
                qCritical() << Q_FUNC_INFO << "invalid value" << a;
        }
        else
            qCritical() << Q_FUNC_INFO << "invalid value" << a;
        has_format = true;
    }
#endif

    return has_format;
}

static bool getFormatFromTag(const QDomElement &elm, QTextCharFormat &format)
{
    return getFormatFromAttributes(elm.tagName(), elm, format);
}


namespace {

class WsXmlLoader {
    rt::Block loadBlock(const QDomElement &elm);
public:
    rt::Fragment load(const QDomElement &elm, bool *ok);
};

}

rt::Block WsXmlLoader::loadBlock(const QDomElement &elm)
{
    QTextBlockFormat format;
    const QString a = elm.attribute("align");
    if (!a.isEmpty())
    {
        if (a == "right")
            format.setAlignment(Qt::AlignRight);
        else if (a == "center")
            format.setAlignment(Qt::AlignHCenter);
        else if (a == "justify")
            format.setAlignment(Qt::AlignJustify);
        else
            qCritical() << Q_FUNC_INFO << "unknown value" << a;
    }

    QList<rt::Chunk> chunks;

    for (QDomNode child = elm.firstChild(); !child.isNull(); child = child.nextSibling())
    {
        if (child.isText())
        {
            QString text = child.toText().data();
            chunks.append(rt::Chunk(text));
        }
        else if (child.isElement())
        {
            QDomElement child_elm = child.toElement();
            QTextCharFormat format;
            if (getFormatFromTag(child_elm, format))
                chunks << rt::Chunk(child_elm.text(), format);
            else
                chunks << rt::Chunk(child_elm.text());
        }
    }

    return rt::Block(chunks, format);
}

rt::Fragment WsXmlLoader::load(const QDomElement &elm, bool *ok)
{
    QList<rt::Block> blocks;

    for (QDomElement child = elm.firstChildElement(); !child.isNull();
         child = child.nextSiblingElement())
    {
        if (child.tagName() != "p")
        {
            qCritical() << "unknown tag" << child.tagName();
            if (ok)
                ok = false;
            continue;
        }

        blocks << loadBlock(child);
    }

    return rt::Fragment(blocks);
}

rt::Fragment rt::Fragment::fromWsXml(const QDomElement &elm, bool *ok)
{
    return WsXmlLoader().load(elm, ok);
}


namespace {

class FormatStack {
    QList<QTextCharFormat> list;

public:
    void push(const QTextCharFormat &fmt)
    {
        if (isEmpty())
        {
            list << fmt;
        }
        else
        {
            QTextCharFormat old = top();
            old.merge(fmt);
            list << old;
        }
    }

    void pop()
    {
        M_ASSERT(!isEmpty());
        list.pop_back();
    }

    const QTextCharFormat &top() const
    {
        M_ASSERT(!isEmpty());
        return list.last();
    }

    bool isEmpty() const
    {
        return list.isEmpty();
    }
};

class MarkupLoader : public QXmlDefaultHandler {
    FormatStack format_stack;
    QList<rt::Block> blocks;
    rt::Block current_block;

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &atts);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    bool characters(const QString &text);
    bool fatalError(const QXmlParseException &exception);

public:
    rt::Fragment load(const QString &markup, bool *ok);
};

struct AttrAccessor {
    const QXmlAttributes &atts;
    AttrAccessor(const QXmlAttributes &atts) : atts(atts) {}
    QString attribute(const QString &name) const { return atts.value(name); }
};

}

static bool getFormatFromTag(const QString &tagName,
                             const QXmlAttributes &atts,
                             QTextCharFormat &format)
{
    AttrAccessor a(atts);
    return getFormatFromAttributes(tagName, a, format);
}

bool MarkupLoader::fatalError(const QXmlParseException &exception)
{
    qWarning() << "Fatal error on line" << exception.lineNumber()
               << ", column" << exception.columnNumber()
               << ":" << exception.message();
    return false;
}

bool MarkupLoader::startElement(const QString &/*namespaceURI*/,
                                const QString &/*localName*/,
                                const QString &qName,
                                const QXmlAttributes &atts)
{
    QTextCharFormat format;
    if (getFormatFromTag(qName, atts, format) || !format_stack.isEmpty())
        format_stack.push(format);
    return true;
}

bool MarkupLoader::endElement(const QString&, const QString&, const QString&)
{
    if (!format_stack.isEmpty())
        format_stack.pop();
    return true;
}

bool MarkupLoader::characters(const QString &text)
{
    if (text.isEmpty())
        return true;

    const QStringList lines = text.split(QRegExp("\\r\\n|\\n")); // XXX
    bool first = true;

    foreach (const QString &s, lines)
    {
        if (!first)
        {
            blocks << current_block;
            current_block = rt::Block();
        }

        if (!s.isEmpty())
        {
            if (format_stack.isEmpty())
                current_block.append(rt::Chunk(s));
            else
                current_block.append(rt::Chunk(s, format_stack.top()));
        }

        first = false;
    }

    return true;
}

rt::Fragment MarkupLoader::load(const QString &markup, bool *ok)
{
    QString fixed = "<span>";
    fixed += markup;
    fixed += "</span>";

    QXmlInputSource input;
    input.setData(fixed);

    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    if (!reader.parse(&input) && ok)
        *ok = false;

    blocks << current_block;
    return rt::Fragment(blocks);
}

rt::Fragment rt::Fragment::fromMarkup(const QString &markup, bool *ok)
{
    return MarkupLoader().load(markup, ok);
}
