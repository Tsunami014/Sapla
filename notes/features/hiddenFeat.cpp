#include "features.hpp"

const QRegularExpression hiddenRe(R"(\[\[((?:.|\n)*?)(?:(?<!\\)::(?<back>(?:.|\n)*?))?\]\])");
QString HiddenFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME) return txt;
    if (s == SIDE_HIDE) return txt.remove(hiddenRe);

    auto it = hiddenRe.globalMatch(txt);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString back = m.captured("back");
        QString repl = "";
        if (!back.isNull()) {
            repl = s == SIDE_BACK ? back : m.captured(1);
        }
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        txt.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return txt;
}
QString HiddenFeat::markup(QString& line) const {
    auto it = hiddenRe.globalMatch(line);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString front = m.captured(1);
        QString back = m.captured("back");
        QString repl = "<b style='color:" + col + "'>[[</b>" + front;
        if (!back.isNull()) {
            repl += "<b style='color:" + col + "'>::</b>" + back;
        }
        repl += "<b style='color:" + col + "'>]]</b>";
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        line.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return line;
}
QMap<QString, QString> HiddenFeat::help() const {
    return {
           {"Always hidden text\n[[]]",
            "Always hides text (so can be seen as a 'comment')\n"
            "E.g. [[This text will NEVER be seen and doesn't affect anything!]]\n"
            "Note features will still do stuff there though (e.g. if you had note templates or something it will still evaluate them)\n"
            "Note templates will not work inside this."
        }, {"Hidden on sides\n[[::]]",
            "Only shows text if it's on a specific side\n"
            "E.g. `[[hi::bye]]` would show `hi` if it's on the front side of a flashcard and `bye` if it's on the back.\n"
            "Leaving sides blank is ok (e.g. `[[front::]]`)\n"
            "Note templates will not work inside this."
        }, {"Randomly hidden\n[[||]]",
            "Randomly selects one of the listed features to use\n"
            "E.g. `[[A||B]]` would show either A or B with a 50% chance\n"
            "This can be combined with hidden on sides, e.g. `[[choice1||choice2front::choice2back||choice3]]`)\n"
            "Note templates will not work inside this."
        }
    };
}

