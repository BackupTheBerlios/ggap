#ifndef MOO_WS_TEXT_BLOCK_H
#define MOO_WS_TEXT_BLOCK_H

#include <moows/wsblock.h>

namespace moo {
namespace ws {


class PromptBlock;
class OutputBlock;
class TextBlock;

class TextBlockBase : public Block {
public:
    QString text() const;
    QString html() const;
    QStringList lines() const;

    QTextCursor cursorAt(int line, int column);
    void newLine(QTextCursor &cr);

    void setText(const QString &text);
    void append(const QString &text);
    void append(const rt::Fragment &fragment);

protected:
    TextBlockBase(bool editable);
    ~TextBlockBase();

protected:
    void added();
    void removed();
};


class TextBlockPrivate;
class TextBlock : public TextBlockBase {
    M_DECLARE_IMPL(TextBlock)

public:
    TextBlock();
    ~TextBlock();

private:
    void setColorScheme(const ColorScheme &scheme);
};


class OutputBlockPrivate;
class OutputBlock : public TextBlockBase {
    M_DECLARE_IMPL(OutputBlock)

public:
    enum Type {
        Stdout,
        Stderr
    };

    OutputBlock(bool is_stdout);
    ~OutputBlock();

    Type type() const;
    PromptBlock *promptBlock();

    void output(const QString &text, bool markup);

private:
    friend class PromptBlock;
    friend class PromptBlockPrivate;
    void setPromptBlock(PromptBlock *pb);
    void setColorScheme(const ColorScheme &scheme);
};


class PromptBlockPrivate;
class PromptBlock : public TextBlockBase {
    Q_OBJECT
    M_DECLARE_IMPL(PromptBlock)

public:
    PromptBlock();
    ~PromptBlock();

    void highlightError(int line, int start_column, int end_column);
    void clearErrors();

    OutputBlock *outputBlock(int type);
    OutputBlock *outputBlock(OutputBlock::Type type) { return outputBlock((int)type); }
    void setOutputBlock(int type, OutputBlock *ob);
    void setOutputBlock(OutputBlock::Type type, OutputBlock *ob) { setOutputBlock((int)type, ob); }

private:
    void setColorScheme(const ColorScheme &scheme);

private Q_SLOTS:
    void docContentsChanged();
};


inline bool isPromptBlock(const Block *b)
{
    return dynamic_cast<const PromptBlock*>(b) != 0;
}

inline bool isOutputBlock(const Block *b)
{
    return dynamic_cast<const OutputBlock*>(b) != 0;
}


} // namespace ws
} // namespace moo

#endif // MOO_WS_TEXT_BLOCK_H
