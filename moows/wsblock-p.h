#ifndef MOO_WS_BLOCK_P_H
#define MOO_WS_BLOCK_P_H

#include "moows/wsblock.h"
#include <QTextBlockUserData>

namespace moo {
namespace ws {

struct WsUserData : public QTextBlockUserData {
    Block *block;
    WsUserData(Block *block) : block(block) {}
};

class BlockPrivate {
    M_DECLARE_PUBLIC(Block)

public:
    BlockPrivate(Block *q);
    virtual ~BlockPrivate();

private:
    TextDocument *doc;
    Block *prev;
    Block *next;
    QTextCursor start;
    QTextCursor end;
    bool editable;
    BlockFormat format;

    friend class EditCommand;
    friend class TextDocument;
    friend class TextDocumentPrivate;
};

} // namespace ws
} // namespace moo

#endif // MOO_WS_BLOCK_P_H
