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
};

MarkdownEdit::MarkdownEdit(QWidget* parent) : QTextEdit(parent) { init(); }
MarkdownEdit::MarkdownEdit(const QString& text, QWidget* parent) : QTextEdit(parent) { init(); setMarkdown(text); }
void MarkdownEdit::init() {
    setFont(getFont(1));
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
            blkCurs.insertHtml(parseMarkdownHtml(line));
        }
        blkCurs.endEditBlock();

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

