// Hide and shuffle features
#include "features.hpp"
#include "base/seedrng.hpp"
#include <deque>

const QRegularExpression scramblRe(R"((?<!\\)\.\.(.*?)\.\.)");
QString ScrambledFeat::replacements(QString& txt, Side s) const {
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
                        std::shuffle(str.begin(), str.end(), getRNG());
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
QString ScrambledFeat::markup(QString& line) const {
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
QMap<QString, QString> ScrambledFeat::help() const {
    return {
           {"Scrambled text\n.. ..",
            "Scrambles each word, keeping the start and end letters and removing capitalisation\n"
            "E.g. `..Hello wOrLd..` might scramble to `Hlelo wlord`"
        }
    };
}


const QRegularExpression shuflRe(R"((?<!\\)>> ?(?<conts>(?:.|\n)+?)(?<end>$| ?(?<!\\)<<))", MO);
struct shflResult {
    bool eol;
    qsizetype start;
    qsizetype end;
};
QString ShuffledFeat::check(QString& txt, QString& err) const {
    std::deque<shflResult> res;
    std::vector<QString> endoflns;
    std::vector<QString> inlns;
    auto it = shuflRe.globalMatch(txt);
    while (it.hasNext()) {
        auto m = it.next();
        bool eol = m.captured("end").isEmpty();
        (eol? endoflns : inlns).push_back(m.captured("conts"));
        res.push_front(shflResult{ eol,
            m.capturedStart(),
            m.capturedEnd()
        });
    }
    auto rng = getRNG();
    std::shuffle(endoflns.begin(), endoflns.end(), rng);
    std::shuffle(inlns.begin(), inlns.end(), rng);
    int offs = 0;
    while (!res.empty()) {
        auto nxt = res.back();
        res.pop_back();
        auto* li = (nxt.eol? &endoflns : &inlns);
        if (li->empty()) {
            Log::Error(MODULE) << "Shuffle failed! How did you manage this?";
            return txt;
        }
        QString repl = li->back();
        li->pop_back();
        int start = nxt.start + offs;
        int end = nxt.end + offs;
        txt.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return txt;
}
QString ShuffledFeat::markup(QString& line) const {
    const QRegularExpression re(R"((?<!\\)(&gt;&gt; ?| ?&lt;&lt;))");
    return line.replace(re, "<b style='color:" + col + "'>\\1</b>");
}
QMap<QString, QString> ShuffledFeat::help() const {
    return {
           {"Shuffled line\n>>",
            "All line shuffles (not both) get shuffled around\n"
            "Matches text until the end of the line"
        }, {"Shuffled text\n>> <<",
            "All text shuffles (not both) get shuffled around\n"
            "Matches text in between >> and <<"
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
                idx = rng_bounded(opts.size());
            } else {
                idx = rng_bounded_seed(qHash(seed), opts.size());
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

