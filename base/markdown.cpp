#include "markdown.hpp"
#include "font.hpp"
#include "../notes/features.hpp"
#include <QTextList>
#include <QTextBlock>
#include <QRegularExpression>

void replace(QString& search, const QString& regex, std::function<QString(QRegularExpressionMatch)> replFn) {
    QRegularExpression re(regex, QRegularExpression::MultilineOption);
    auto it = re.globalMatch(search);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString repl = replFn(m);

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        search.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
}

QString parseMarkdownHtml(QString txt) {
    QString esc = txt.toHtmlEscaped();

    esc.replace("\t", "    ");

    // Lists
    replace(esc, R"(^( *)([*\-+])[ \t]+(.+)$)", [](QRegularExpressionMatch m) {
        QString bullet;
        switch (m.captured(2)[0].unicode()) {
            case '-':
                bullet = "‣";
                break;
            case '+':
                bullet = "✦";
                break;
            default:
                bullet = "•";
        }

        return QString("%1%2 %3")
            .arg(m.captured(1)).arg(bullet).arg(m.captured(3));
    });

    // Bold & italic
    esc.replace(QRegularExpression(R"((?:\*\*([^*]+?)\*\*|__([^_]+?)__))"), "<b>\\1</b>")
       .replace(QRegularExpression(R"((?:\*([^*]+?)\*|_([^_]+?)_))"), "<i>\\1</i>");

    // Now parse all the features
    for (auto& f : Feats) {
        esc = f->markup(esc);
    }

    // Fix spaces and finish
    replace(esc, R"(^ +)", [](QRegularExpressionMatch m) {
        int count = m.capturedLength();
        return QString("&nbsp;").repeated(count);
    });
    esc.replace("\n", "<br/>");
    return esc;
}

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
    setFont(getFont(1));
    connect(this, &QTextEdit::selectionChanged, this, [this](){
        QSignalBlocker blocker(this);
        SaveCursor savCurs(this);
        updateTxt(false, false);
    });
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
            txt += data->orig + "\n";
        }
        block = block.next();
    }
    return txt;
}

void MarkdownEdit::keyPressEvent(QKeyEvent *event) {
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
    QSignalBlocker blocker(this);
    QTextEdit::mousePressEvent(event);
    SaveCursor savCurs(this);
    updateTxt(false, false);
}
void MarkdownEdit::focusOutEvent(QFocusEvent *event) {
    QSignalBlocker blocker(this);
    QTextEdit::focusOutEvent(event);
    QTextCursor c = textCursor();
    c.clearSelection();
    setTextCursor(c);
    updateTxt(false, false);
}

void MarkdownEdit::updateTxt(bool save, bool orig) {
    QTextCursor curs = textCursor();
    bool focus = this->hasFocus();
    bool hasSel = curs.hasSelection();
    QTextDocument* doc = document();
    int sBlk = doc->findBlock(curs.selectionStart()).blockNumber();
    int eBlk;
    if (hasSel) {
        int selEndPos = curs.selectionEnd();
        if (selEndPos > curs.selectionStart()) selEndPos = selEndPos - 1;
        eBlk = doc->findBlock(selEndPos).blockNumber();
    } else {
        eBlk = doc->findBlock(curs.selectionEnd()).blockNumber();
    }

    QTextCharFormat plainfmt;
    plainfmt.setFont(document()->defaultFont());
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

        bool plain = focus && (orig ||
            (!hasSel && bnum == curs.blockNumber()) ||
            (hasSel && bnum >= sBlk && bnum <= eBlk)
        );

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

