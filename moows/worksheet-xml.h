#ifndef MOO_WORKSHEET_XML_H
#define MOO_WORKSHEET_XML_H

#include <QList>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QDomElement>
#include <QSharedDataPointer>

namespace moo {
namespace ws {
namespace rt {

class Chunk {
    QString text_;
    QTextCharFormat format_;
    bool plain_text;

public:
    Chunk(const QString &text, const QTextCharFormat &format) :
        text_(text), format_(format), plain_text(false)
    {
    }

    explicit Chunk(const QString &text, const QTextCharFormat *format = 0) :
        text_(text), format_(), plain_text(true)
    {
        if (format)
        {
            format_ = *format;
            plain_text = false;
        }
    }

    const QString &text() const { return text_; }
    const QTextCharFormat &format() const { return format_; }
    int length() const { return text_.size(); }
    bool isEmpty() const { return text_.isEmpty(); }

    bool isPlainText() const { return plain_text || text_.isEmpty(); }
};

class Block {
    QList<Chunk> chunks_;
    QTextBlockFormat format_;

public:
    Block()
    {
    }

    Block(const QList<Chunk> &chunks, const QTextBlockFormat &format) :
        chunks_(chunks), format_(format)
    {
    }

    explicit Block(const QList<Chunk> &chunks) :
        chunks_(chunks)
    {
    }

    explicit Block(const QString &text, const QTextBlockFormat &format) :
        format_(format)
    {
        chunks_ << Chunk(text);
    }

    explicit Block(const QString &text, const QTextCharFormat *format = 0)
    {
        chunks_ << Chunk(text, format);
    }

    void append(const Block &other)
    {
        chunks_ << other.chunks_;
        format_.merge(other.format_); // XXX
    }

    void append(const Chunk &c)
    {
        chunks_ << c;
    }

    int count() const { return chunks_.size(); }
    const Chunk &chunk(int n) const { return chunks_.at(n); }
    const QTextBlockFormat &format() const { return format_; }

    int length() const
    {
        int l = 0;
        foreach (const Chunk &c, chunks_)
            l += c.length();
        return l;
    }

    bool isPlainText() const
    {
        if (format_.hasProperty(QTextFormat::BlockAlignment))
            return false;
        foreach (const Chunk &c, chunks_)
            if (!c.isPlainText())
                return false;
        return true;
    }

    bool isEmpty() const
    {
        foreach (const Chunk &c, chunks_)
            if (!c.isEmpty())
                return false;
        return true;
    }
};

class FragmentData;
class Fragment {
    QSharedDataPointer<FragmentData> d;

public:
    Fragment();
    ~Fragment();
    explicit Fragment(const QString &text, const QTextCharFormat *format = 0);
    explicit Fragment(const QList<Block> &blocks);
    explicit Fragment(const Block &block);
    explicit Fragment(const QTextCursor &cr);

    Fragment(const Fragment &fragment);
    Fragment &operator = (const Fragment &fragment);

    void append(const Fragment &other);
    void prepend(const Fragment &other);

    bool isPlainText() const;
    int blockCount() const;
    const Block &block(int n) const;

    int length() const;
    bool isEmpty() const;

    // free form, doesn't care about <p> tags, does care whitespace
    static Fragment fromMarkup(const QString &markup, bool *ok = 0);

    // must be a sequence of <p> tags
    static Fragment fromWsXml(const QDomElement &elm, bool *ok = 0);
};

}

}
}

#endif // MOO_WORKSHEET_XML_H
