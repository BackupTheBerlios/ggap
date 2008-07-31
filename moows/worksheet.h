#ifndef MOO_WORKSHEET_H
#define MOO_WORKSHEET_H

#include <moo-pimpl.h>
#include <QTextEdit>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QActionGroup>
#include <QDomDocument>
#include <QXmlStreamWriter>

namespace moo {
namespace ws {

class Block;
class TextDocument;
class TextBlockBase;

class WorksheetPrivate;
class Worksheet : public QTextEdit
{
    Q_OBJECT
    M_DECLARE_IMPL(Worksheet)

    typedef QTextEdit ParentClass;

public:
    Worksheet(QWidget *parent = 0);
    ~Worksheet();

    void reset();

    Block *firstBlock();
    Block *lastBlock();
    const Block *firstBlock() const;
    const Block *lastBlock() const;
    Block *blockAtCursor();
    Block *blockAtCursor(const QTextCursor &cr);

    TextDocument &document();
    const TextDocument &document() const;

    QAction *actionFontName(QObject *parent);
    QAction *actionFontSize(QObject *parent);
    QAction *actionFontButton(QObject *parent);
    QAction *actionBold(QObject *parent);
    QAction *actionItalic(QObject *parent);
    QAction *actionUnderline(QObject *parent);
    QActionGroup *actionGroupJustify(QObject *parent);
    QActionGroup *actionGroupSupSubScript(QObject *parent);
    QAction *actionTextColor(QObject *parent);

    void setCurrentCharFormat(const QTextCharFormat &fmt);
    void setCurrentBlockFormat(const QTextBlockFormat &fmt);
public Q_SLOTS:
    void emitCursorPositionChanged();
    void setTrackCurrentFormat(bool track);

protected:
    void startInput(const QString &prompt);
    void setAcceptingInput(bool accepting);
    void resumeInput(int line, int column);
    void continueInput();
    void highlightError(int line, int firstColumn, int lastColumn, const QString &message);
    void writeError(const QString &msg);
    void writeOutput(const QString &text);
    void writeMarkup(const QString &markup);
    void addHistory(const QString &string);
    void resetHistory();
    bool loadXml(const QString &filename, QString &error);
    bool loadXml(const QDomDocument &xml, QString &error);
    void format(QXmlStreamWriter &xml);
    void formatBlock(QXmlStreamWriter &xml, TextBlockBase *text_block);

public Q_SLOTS:
    void addPromptBlock(bool afterCursor);
    void addPromptBlockBeforeCursor();
    void addPromptBlockAfterCursor();
    void addTextBlock(bool afterCursor);
    void addTextBlockBeforeCursor();
    void addTextBlockAfterCursor();
    void deleteBlock();

public Q_SLOTS:
    // These override QTextEdit's functionality, they aren't
    // virtual in QTextEdit
    void cut();
    void copy();
    void paste();
    void deleteSelected();
    void undo();
    void redo();
    void beep();

Q_SIGNALS:
    void currentCharFormatChanged(bool, const QTextCharFormat&);
    void currentBlockFormatChanged(bool, const QTextBlockFormat&);
    void highlighted(const QString &link);

private:
    virtual void activateUrl(const QString &anchor);
    virtual void processInput(const QStringList &lines);
    void doWriteOutput(const QString &text, bool is_stdout, bool is_markup);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void inputMethodEvent(QInputMethodEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void insertFromMimeData(const QMimeData *source);
};

} // namespace ws
} // namespace moo

#endif // MOO_WORKSHEET_H
