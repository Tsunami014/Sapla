#include "markdown.hpp"
#include "font.hpp"
#include <QTextList>
#include <QTextBlock>


class MarkdownBlockData : public QTextBlockUserData {
public:
    QString orig;
    bool plain;
};

SaveCursor::SaveCursor(QTextEdit* parent) : QObject(parent), edit(parent) {
    QTextCursor curs = edit->textCursor();
    anchor = save(curs.anchor());
    pos = save(curs.position());
}
SaveCursor::~SaveCursor() {
    QTextCursor ncurs(edit->document());
    ncurs.setPosition(load(anchor));
    ncurs.setPosition(load(pos), QTextCursor::KeepAnchor);
    edit->setTextCursor(ncurs);
}
SaveCursor::Point SaveCursor::save(int thing) {
    QTextBlock blk = edit->document()->findBlock(thing);
    return {blk.blockNumber(), thing - blk.position()};
}
int SaveCursor::load(Point saved) {
    return edit->document()->findBlockByNumber(saved.first).position() + saved.second;
}

MarkdownEdit::MarkdownEdit(QWidget* parent) : QTextEdit(parent) { init(); }
MarkdownEdit::MarkdownEdit(const QString& text, QWidget* parent) : QTextEdit(parent) { init(); setMarkdown(text); }
void MarkdownEdit::init() {
    isBtn = false;
    lastCol = -1;
    setFont(getFont(1));
    connect(this, &QTextEdit::selectionChanged, this, [this](){
        QSignalBlocker blocker(this);
        SaveCursor savCurs(this);
        updateTxt(false, false);
    });
}

void MarkdownEdit::setButton(bool enabled) {
    isBtn = enabled;
    if (enabled) {
        setTextInteractionFlags(Qt::NoTextInteraction);
        setCursor(Qt::PointingHandCursor);
        viewport()->setCursor(Qt::PointingHandCursor);
        clearFocus();
    } else {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        unsetCursor();
        viewport()->unsetCursor();
    }
}

void MarkdownEdit::setMarkdown(const QString& text) {
    QSignalBlocker blocker(this);
    setPlainText(text);
    updateTxt(true, false);
    update();
}
QString MarkdownEdit::getMarkdown() {
    QString txt = "";
    QTextBlock block = document()->begin();
    while (block.isValid()) {
        int bnum = block.blockNumber();
        auto* data = static_cast<MarkdownBlockData*>(block.userData());
        if (data) {
            txt += data->orig;
            if (block.next().isValid()) {
                txt += "\n";
            }
        }
        block = block.next();
    }
    return txt;
}

void MarkdownEdit::keyPressEvent(QKeyEvent *event) {
    if (isBtn) return;
    bool undo = event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Z;
    bool redo = (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Y) ||
        (event->modifiers() & Qt::ControlModifier & Qt::ShiftModifier && event->key() == Qt::Key_Z);
    // TODO: Undo and redo
    if (redo) {
        event->accept();
        return;
    } else if (undo) {
        event->accept();
        return;
    }
    QSignalBlocker blocker(this);
    updateTxt(false, true);
    QTextEdit::keyPressEvent(event);
    updateTxt(true, false);
    blocker.unblock();
    emit textChanged();
}
void MarkdownEdit::mousePressEvent(QMouseEvent* event) {
    QTextEdit::mousePressEvent(event);
    if (isBtn) {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
        return;
    }
    QSignalBlocker blocker(this);
    SaveCursor savCurs(this);
    updateTxt(false, false);
}
void MarkdownEdit::mouseDoubleClickEvent(QMouseEvent* event) {
    if (isBtn && event->button() == Qt::LeftButton) {
        emit clicked();
    }
}
void MarkdownEdit::focusOutEvent(QFocusEvent *event) {
    QTextEdit::focusOutEvent(event);
    if (isBtn) return;
    QSignalBlocker blocker(this);
    QTextCursor c = textCursor();
    c.clearSelection();
    lastCol = c.position() - c.block().position();
    setTextCursor(c);
    updateTxt(false, false);
}
void MarkdownEdit::focusInEvent(QFocusEvent *event) {
    QTextEdit::focusInEvent(event);
    if (isBtn) return;
    QSignalBlocker blocker(this);
    updateTxt(false, false);
    if (lastCol != -1) {
        QTextCursor c = textCursor();
        int newPos = c.block().position() + qMin(lastCol, c.block().length() - 1);
        c.setPosition(newPos);
        setTextCursor(c);
        lastCol = -1;
    }
}

void MarkdownEdit::insertMarkdown(QString txt, QString cursSub) {
    if (!isEnabled()) return;
    setFocus();
    QSignalBlocker blocker(this);
    QTextCursor c = textCursor();
    if (lastCol != -1) {
        int newPos = c.block().position() + qMin(lastCol, c.block().length() - 1);
        c.setPosition(newPos);
    }
    updateTxt(false, true);
    int pos;
    if (cursSub != "") {
        pos = txt.indexOf(cursSub);
    } else {
        pos = -1;
    }
    if (pos != -1) {
        c.insertText(txt.left(pos));
        int finalPos = c.position();
        c.insertText(txt.mid(pos + cursSub.length()));

        c.setPosition(finalPos);
    } else {
        c.insertText(txt);
    }
    setTextCursor(c);
    lastCol = c.position() - c.block().position();
    updateTxt(true, false);
    blocker.unblock();
    emit textChanged();
}

void MarkdownEdit::refresh() {
    QSignalBlocker blocker(this);
    updateTxt(false, false);
}

void MarkdownEdit::updateTxt(bool save, bool orig) {
    QTextDocument* doc = document();
    bool focus;
    int sBlk; int eBlk;
    if (isBtn) {
        focus = false;
        sBlk = -1; eBlk = -1;
    } else {
        focus = this->hasFocus();
        QTextCursor curs = textCursor();
        if (curs.hasSelection()) {
            sBlk = doc->findBlock(curs.selectionStart()).blockNumber();
            int selEndPos = curs.selectionEnd();
            if (selEndPos > curs.selectionStart()) selEndPos = selEndPos - 1;
            eBlk = doc->findBlock(selEndPos).blockNumber();
        } else {
            sBlk = curs.blockNumber();
            eBlk = sBlk;
        }
    }

    QTextCharFormat plainfmt;
    plainfmt.setFont(doc->defaultFont());
    plainfmt.setFontWeight(QFont::Normal);
    plainfmt.setFontItalic(false);

    QTextBlock block = doc->begin();
    while (block.isValid()) {
        int bnum = block.blockNumber();
        auto* data = static_cast<MarkdownBlockData*>(block.userData());
        bool newData = !data;
        if (newData) {
            data = new MarkdownBlockData();
            block.setUserData(data);
        }

        if (newData || (save && block.text() != data->orig)) {
            data->orig = block.text();
            data->plain = true;
        }
        QString line = data->orig;

        bool plain = focus && (orig || (bnum >= sBlk && bnum <= eBlk));

        if (!(
            // If it is supposed to be plain and already is and is the correct text, skip
            (plain && data->plain && line == block.text()) ||
            // If it is supposed to be parsed and already is, skip (users shouldn't be able to edit parsed data)
            (!plain && !data->plain)
        )) {
            data->plain = plain;
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
            if (plain) {
                blkCurs.insertText(line, plainfmt);
            } else {
                blkCurs.insertHtml(parseMarkdownHtml(line));
            }
            blkCurs.endEditBlock();
        }

        block = block.next();
    }
}


MarkdownGraphicsText::MarkdownGraphicsText(QGraphicsItem* parent) : QGraphicsTextItem(parent) {}
MarkdownGraphicsText::MarkdownGraphicsText(const QString& text, QGraphicsItem* parent) : QGraphicsTextItem(parent) {
    setMarkdown(text);
}
void MarkdownGraphicsText::setMarkdown(const QString& text) {
    setHtml(parseMarkdownHtml(text));
}

