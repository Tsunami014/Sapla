#include "markdown.hpp"
#include "font.hpp"
#include <QTextBlock>

class MarkdownBlockData : public QTextBlockUserData {
public:
    QString orig;
};

MarkdownEdit::MarkdownEdit(QWidget* parent) : QTextEdit(parent) { init(); }
MarkdownEdit::MarkdownEdit(const QString& text, QWidget* parent) : QTextEdit(parent) { init(); setMarkdown(text); }
void MarkdownEdit::init() {
    setFont(getFont());
}

void MarkdownEdit::setMarkdown(const QString& text) {
    QSignalBlocker blocker(this);
    setPlainText(text);
    updateTxt(true, false);
    update();
}
QString MarkdownEdit::getMarkdown() {
    QSignalBlocker blocker(this);
    updateTxt(false, true);
    QString txt = toPlainText();
    updateTxt(false, false);
    return txt;
}

void MarkdownEdit::keyPressEvent(QKeyEvent *event) {
    QSignalBlocker blocker(this);
    updateTxt(false, true);
    QTextEdit::keyPressEvent(event);
    updateTxt(true, false);
    blocker.unblock();
    emit textChanged();
}
void MarkdownEdit::mousePressEvent(QMouseEvent* event) {
    QSignalBlocker blocker(this);
    QTextEdit::mousePressEvent(event);
    updateTxt(false, false);
}
void MarkdownEdit::focusOutEvent(QFocusEvent *event) {
    QSignalBlocker blocker(this);
    QTextEdit::focusOutEvent(event);
    updateTxt(false, false);
}

void MarkdownEdit::updateTxt(bool save, bool orig) {
    QTextCursor curs = textCursor();
    bool focus = this->hasFocus();
    bool hasSel = curs.hasSelection();
    QTextDocument* doc = document();
    int sBlk = doc->findBlock(curs.selectionStart()).blockNumber();
    int eBlk = doc->findBlock(curs.selectionEnd()).blockNumber();

    QTextCharFormat plainfmt;
    plainfmt.setFont(document()->defaultFont());
    plainfmt.setFontWeight(QFont::Normal);
    plainfmt.setFontItalic(false);

    QTextBlock block = doc->begin();
    while (block.isValid()) {
        int bnum = block.blockNumber();
        auto* data = static_cast<MarkdownBlockData*>(block.userData());
        if (!data) {
            data = new MarkdownBlockData();
            block.setUserData(data);
        }

        if (save) data->orig = block.text();
        QString line = data->orig;
        QTextCursor blkCurs(block);

        int start = block.position();
        int len = block.length() - 1;
        blkCurs.setPosition(start);
        if (len > 0) {
            blkCurs.setPosition(start + len, QTextCursor::KeepAnchor);
        } else {
            blkCurs.setPosition(start);  // If empty line have a zero length selection
        }

        blkCurs.beginEditBlock();
        if (focus && (orig ||
            (!hasSel && bnum == curs.blockNumber()) ||
            (hasSel && bnum >= sBlk && bnum <= eBlk)
        )) {
            if (blkCurs.selectedText() != line) blkCurs.insertText(line, plainfmt);
        } else {
            // TODO: Process markdown into html
            blkCurs.insertHtml(line);
        }
        blkCurs.endEditBlock();

        block = block.next();
    }
}

