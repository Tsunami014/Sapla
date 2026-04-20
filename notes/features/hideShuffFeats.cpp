// Hidden and shuffle features
#include "features.hpp"
#include <QRandomGenerator>
#include <QDateTime>

const QRegularExpression scramblRe(R"((?<!\\)\.\.(.*?)\.\.)");
QString ShuffledFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME || s == SIDE_GETFC) return txt;
    if (s == SIDE_HIDE) return txt.remove(scramblRe);

    auto it = scramblRe.globalMatch(txt);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString conts = m.captured(1);
        QString out;
        if (s == SIDE_BACK) {
            out = conts;
        } else {
            QStringList groups;
            for (auto s : conts.split(' ')) {
                auto ln = s.length();
                if (ln <= 3) {
                    groups.append(s);
                } else {
                    QString str = s.sliced(1, ln-2);
                    if (!str.isUpper()) str = str.toLower();
                    if (
                        std::all_of(str.begin(), str.end(),
                            [&](QChar c){ return c == str[0]; })
                    ) {
                        groups.append(s);
                        continue;
                    }
                    QString strorig = str;
                    do {
                        std::shuffle(str.begin(), str.end(), *QRandomGenerator::global());
                    } while (str == strorig);
                    groups.append(
                        s[0] + str + s[ln-1]
                    );
                }
            }
            out = groups.join(' ');
        }
        QString repl = "**\x01" + out + "\x01**";

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        txt.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return txt;
}
QString ShuffledFeat::markup(QString& line) const {
    auto it = scramblRe.globalMatch(line);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString dots = "<b style='color:" + col + "'>..</b>";
        QString repl = dots+m.captured(1)+dots;

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        line.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return line;
}
QMap<QString, QString> ShuffledFeat::help() const {
    return {
           {"Scrambled text\n.. ..",
            "Scrambles each word, keeping the start and end letters and removing capitalisation\n"
            "E.g. `..Hello wOrLd..` might scramble to `Hlelo wlord`"
        }
    };
}


struct hiddenResult {
    int start;
    int end;
    QString contents;
};

const QRegularExpression hiddenRe(R"((?<!\\)\[\[((?:.|\n)*?)(?<!\\)\]\])");
QString HiddenFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME || s == SIDE_GETFC) return txt;
    if (s == SIDE_HIDE) return txt.remove(hiddenRe);
    auto tse = QDateTime::currentMSecsSinceEpoch();

    auto it = hiddenRe.globalMatch(txt);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString conts = m.captured(1);
        QString seed;
        if (conts.contains("##")) {
            int idx = conts.indexOf("##");
            seed = conts.sliced(idx+2);
            conts.slice(0, idx);
        }
        QString repl;
        if (conts.contains("::") || conts.contains("//")) {
            QStringList opts = conts.split("//");
            uint idx;
            if (seed.isNull()) {
                idx = QRandomGenerator::global()->bounded(opts.size());
            } else {
                idx = QRandomGenerator(tse+qHash(seed)).bounded(opts.size());
            }
            QString item = opts[idx];

            if (item.contains("::")) {
                QStringList sides = item.split("::");
                repl = s == SIDE_BACK ? sides[1] : sides[0];
            } else {
                repl = item;
            }
        } else {
            repl = "";
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
        QString conts = m.captured(1);
        conts
            .replace("//", "<b style='color:" + col + "'>//</b>")
            .replace("##", "<b style='color:" + col + "'>##</b>")
            .replace("::", "<b style='color:" + col + "'>::</b>")
        ;
        QString repl =
            "<b style='color:" + col + "'>[[</b>" +
            conts +
            "<b style='color:" + col + "'>]]</b>"
        ;
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
            "Note features will still do stuff there though (e.g. if you had note templates or something it will still evaluate them)"
        }, {"Hidden on sides\n[[::]]",
            "Only shows text if it's on a specific side\n"
            "E.g. `[[hi::bye]]` would show `hi` if it's on the front side of a flashcard and `bye` if it's on the back.\n"
            "Leaving sides blank is ok (e.g. `[[front::]]`)"
        }, {"Randomly hidden\n[[//##]]",
            "Randomly selects one of the listed features to use\n"
            "E.g. `[[A//B]]` would show either A or B with a 50% chance\n"
            "This can be combined with hidden on sides, e.g. `[[choice1//choice2front::choice2back//choice3]]`)\n"
            "Using ## will set the seed so you can have the same result for different random choices (e.g. `[[1//2##a]] [[1//2##a]]`)\n"
            "The seed can be any text or number input and MUST BE AT THE END (`[[1##seed//2]]` will use a seed of `seed//2`)."
        }
    };
}

