#ifndef MOO_WORKSHEET_XML_P_H
#define MOO_WORKSHEET_XML_P_H

#include "moows/worksheet-xml.h"
#include <QSharedData>

namespace moo {
namespace ws {
namespace rt {

class FragmentData : public QSharedData {
    QList<Block> blocks;

public:
    FragmentData() {}

    FragmentData(const QString &text, const QTextCharFormat *format = 0)
    {
        const QStringList lines = text.split(QRegExp(QString::fromLatin1("\\r\\n|\\n|") + QString(QChar::LineSeparator)));
        foreach (const QString &l, lines)
            blocks << Block(l, format);
    }

    FragmentData(const QList<Block> &blocks) :
        blocks(blocks)
    {
    }

    void append(const FragmentData &other)
    {
        if (blocks.isEmpty())
        {
            blocks = other.blocks;
            return;
        }

        if (other.blocks.isEmpty())
            return;

        blocks.last().append(other.blocks.first());

        for (int i = 1; i < other.blocks.size(); ++i)
            blocks << other.blocks.at(i);
    }

    void prepend(const FragmentData &other)
    {
        FragmentData tmp = blocks;
        blocks = other.blocks;
        append(tmp);
    }

    bool isPlainText() const
    {
        foreach (const Block &b, blocks)
            if (!b.isPlainText())
                return false;
        return true;
    }

    int blockCount() const { return blocks.size(); }
    const Block &block(int n) const { return blocks.at(n); }

    int length() const
    {
        int l = 0;
        foreach (const Block &b, blocks)
            l += b.length();
        if (!blocks.isEmpty())
            l += blocks.size() - 1;
        return l;
    }

    bool isEmpty() const
    {
        return blocks.isEmpty() || (blocks.size() == 1 && blocks[0].isEmpty());
    }
};

}
}
}

#endif // MOO_WORKSHEET_XML_P_H
