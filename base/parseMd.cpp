#include "../core.hpp"
#include "../notes/features.hpp"
#include <QRegularExpression>

#define STATIC_RE(pattern) ([]() -> const QRegularExpression& { \
    static const QRegularExpression re(pattern, QRegularExpression::MultilineOption); \
    return re; \
}())

void replace(QString& search, const QRegularExpression& re, std::function<QString(QRegularExpressionMatch)> replFn) {
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

const QString codeRepl = "<span style='"
    "background-color: grey;"
    "font-family: monospace;"
"'>\\1</span>";
const QString hlRepl = "<span style='background-color: %1;'>"
    "&nbsp;\\1&nbsp;"
"</span>";
const QString quoteRepl = "<span style='"
    "color: lightgray;"
    "font-style: italic;"
"'>\\1▎\\2</span>";
QString parseMarkdownHtml(QString txt) {
    QString esc = txt.toHtmlEscaped();

    esc
       .replace("\t", "    ")
       .replace("\\\\", "⧵")  // To escape backslashes themselves, replace it with a character that won't be picked up
       .replace("\\n", "\n")
    ;

    // Lists
    replace(esc, STATIC_RE(R"(^( *(?:&gt; *)?)([*\-+])[ \t]+(.+)$)"), [](QRegularExpressionMatch m) {
    // (?:> {1,2})? *
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

    // > Block quote
    esc.replace(STATIC_RE(R"(^( *)&gt; {1,2}(.+)$)"), quoteRepl);

    // *italic*, **bold**, _underline_, `code`, ==highlight==, ~~strikethrough~~
    esc
       .replace(STATIC_RE(R"((?<!\\)\*\*([^*]*[^`\\])\*\*)"), "<b>\\1</b>")
       .replace(STATIC_RE(R"((?<!\\)\*([^*]*[^`\\])\*)"), "<i>\\1</i>")
       .replace(STATIC_RE(R"((?<!\\)_([^_]*[^`\\])_)"), "<u>\\1</u>")
       .replace(STATIC_RE(R"((?<!\\)`([^`]*[^`\\])`)"), codeRepl)
       .replace(STATIC_RE(R"((?<!\\)==([^=]*[^=\\])==)"), hlRepl.arg(MG->styls.mdHlCol))
       .replace(STATIC_RE(R"((?<!\\)~~([^~]*[^~\\])~~)"), "<s>\\1</s>");

    // Now parse all the features
    for (auto& f : Feats) {
        esc = f->markup(esc);
    }

    // Fix spaces and finish
    int i = 0;
    while (i < esc.size() && esc.at(i) == ' ') i++;
    esc = QString("&nbsp;").repeated(i) + esc.mid(i)
        .replace("\n", "<br/>")
        .replace("\\", "");  // Get rid of backslashes that are only there for escaping stuff (the \\ was handled at the start)
    return esc;
}
