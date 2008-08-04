#ifndef MOO_WS_BLOCK_FORMAT_H
#define MOO_WS_BLOCK_FORMAT_H

#include <QPainter>
#include <QTextCharFormat>
#include <QTextBlockFormat>

namespace moo {
namespace ws {

class BlockFormat {
    QPen pen;

public:
    QTextCharFormat charFormat() const
    {
        QTextCharFormat fmt;
        fmt.setForeground(pen.brush());
        return fmt;
    }

    QTextBlockFormat blockFormat() const
    {
        QTextBlockFormat fmt;
        fmt.setLeftMargin(10);
        return fmt;
    }

    void applyTo(QPainter &p) const
    {
        p.setPen(pen);
    }

    void setForeground(const QColor &color)
    {
        pen.setColor(color);
    }
};

struct ColorScheme {
    enum NamedColor {
        Prompt,
        Stdout,
        Stderr,
        Text
    };

    QColor color(NamedColor name) const { return colors[name]; }

    ColorScheme()
    {
        defaultInit();
    }

private:
    void defaultInit()
    {
        colors[Prompt] = Qt::darkRed;
        colors[Stdout] = Qt::darkGreen;
        colors[Stderr] = Qt::red;
        colors[Text] = Qt::darkBlue;
    }

    QColor colors[4];
};

}
}

#endif // MOO_WS_BLOCK_FORMAT_H
