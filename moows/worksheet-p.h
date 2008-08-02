#ifndef MOO_WORKSHEET_P_H
#define MOO_WORKSHEET_P_H

#include "moows/worksheet.h"
#include "moows/wstextdocument.h"
#include "moows/wstextblock.h"
#include <QtGui>

class QDomElement;

namespace moo {
namespace ws {

class WorksheetPrivate : public QObject {
    Q_OBJECT
    M_DECLARE_PUBLIC(Worksheet)

    Worksheet *ws;
    TextDocument doc;
    QPointer<PromptBlock> input;
    QPointer<OutputBlock> output[2];

    QStringList history;
    int history_ptr;
    QString history_tmp;

    bool in_input;

public:
    WorksheetPrivate(Worksheet *ws) :
        QObject(ws), pub(ws), ws(ws),
        history(), history_ptr(-1), history_tmp(),
        in_input(false),
        hand_cursor(false),
        track_format(false), char_fmt_valid(false),
        format_timer_started(false)
    {
    }

    Block *blockAtCursor(const QTextCursor &cr) { return Block::atCursor(cr); }
    Block *blockAtCursor() { return blockAtCursor(ws->textCursor()); }

    PromptBlock *promptBlockAtCursor(const QTextCursor &cr) {
        return dynamic_cast<PromptBlock*>(blockAtCursor(cr));
    }

    Block *addBlock(bool prompt, bool afterCursor);

    int horizontalOffset() const
    {
        QScrollBar *s = ws->horizontalScrollBar();
        return s->value();
    }

    int verticalOffset() const
    {
        QScrollBar *s = ws->verticalScrollBar();
        return s->value();
    }

private:
    void commitInput();
    void beep();
    bool isAcceptingInput() const;

    void setHistory(const QStringList &items);
    void history_go(bool forth);
    void historyNext();
    void historyPrev();

    void load_input(const QDomElement &elm, QMap<QString, Block*> &id_map, QMap<PromptBlock*, QStringList> &output_map);
    void load_output(const QDomElement &elm, QMap<QString, Block*> &id_map);
    void load_text(const QDomElement &elm, QMap<QString, Block*> &id_map);
    void load_content(const QDomElement &elm);
    void load_history(const QDomElement &elm);
    void load_block_content(const QDomElement &elm, TextBlockBase *block);

    bool hand_cursor;
    QString highlighted_anchor;

    void highlighted(const QString &newAnchor, bool mouse)
    {
        if (mouse)
        {
            bool need_hand_cursor = !newAnchor.isEmpty();

            if (hand_cursor != need_hand_cursor)
            {
                hand_cursor = need_hand_cursor;
                if (need_hand_cursor)
                    ws->viewport()->setCursor(Qt::PointingHandCursor);
                else
                    ws->viewport()->setCursor(Qt::IBeamCursor);
            }
        }

        if (newAnchor != highlighted_anchor)
        {
            highlighted_anchor = newAnchor;
            emit ws->highlighted(newAnchor);
        }
    }

    QString anchorAtCursor() const
    {
        const QTextCursor cr = ws->textCursor();
        if (cr.hasSelection())
            return QString();
        else
            return cr.charFormat().anchorHref();
    }

    bool track_format;
    bool char_fmt_valid;
    QTextCharFormat char_fmt;
    bool format_timer_started;
    const QTextCharFormat *charFormat() const { return char_fmt_valid ? &char_fmt : 0; }
    void setCurrentCharFormat(const QTextCharFormat &fmt);
    void setCurrentBlockFormat(const QTextBlockFormat &fmt);
    void queueFormatChanged();

private Q_SLOTS:
    void emitFormatChanged();
    void cursorPositionChanged();

private:
    void cut();
    void copy();
    void paste();

    bool canPaste() const;
    QMenu *createContextMenu(QContextMenuEvent *event);
    void insertFromMimeData(const QMimeData *data);
    bool keyPressEvent(QKeyEvent *event);
    bool cursorMoveKeyEvent(QKeyEvent *event, bool &handled);
};

} // namespace ws
} // namespace moo

#endif // MOO_WORKSHEET_P_H
