// Hide and shuffle features
#include "features.hpp"
#include "base/seedrng.hpp"
#include <deque>

const QRegularExpression scramblRe(R"((?<!\\)\.\.(.*?[^\\\n])\.\.)");
QString ScrambledFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME || s == SIDE_GETFC) return txt;
    if (s == SIDE_HIDE) return txt.remove(scramblRe);
    auto rng = getRNG("Scramble");

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
                        std::shuffle(str.begin(), str.end(), rng);
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
const QRegularExpression shuflBasicRe(R"((?<!\\)(&gt;&gt; ?| ?&lt;&lt;))");
struct shflResult {
    bool eol;
    qsizetype start;
    qsizetype end;
};
QString ShuffledFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME) return txt;
    if (s == SIDE_HIDE) return txt.remove(shuflBasicRe);
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
    auto rng = getRNG("Shuffle");
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
    return line.replace(shuflBasicRe, "<b style='color:" + col + "'>\\1</b>");
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


struct hiddenMatch {
    std::vector<QStringList> opts;
    QString seed;
};
std::pair<QString, qsizetype> _matchHiddensInner(qsizetype idx, const QString& out, std::function<QString(hiddenMatch)> replfn) {
    std::vector<QStringList> opts;
    QStringList cur;
    QString sofar;
    bool seed = false;
    bool ended = false;
    while (idx < out.length()) {
        QChar c = out.at(idx++);
        if (c == '\\') {
            sofar += '\\';
            if (idx < out.length()) { sofar += out.at(idx++); }
        } else if (c == '[' && idx < out.length() && out.at(idx) == '[') {
            const auto o = _matchHiddensInner(idx+1, out, replfn);
            if (o.second == -1) {
                sofar += c;
                continue;
            }
            sofar += o.first;
            idx = o.second;
        } else if (!seed && c == ':' && idx < out.length() && out.at(idx) == ':') {
            idx++;
            cur.push_back(sofar);
            sofar = {};
        } else if (!seed && c == '/' && idx < out.length() && out.at(idx) == '/') {
            idx++;
            cur.push_back(sofar);
            sofar = {};
            opts.push_back(cur);
            cur = {};
        } else if (!seed && c == '#' && idx < out.length() && out.at(idx) == '#') {
            idx++;
            cur.push_back(sofar);
            sofar = {};
            opts.push_back(cur);
            cur = {};
            seed = true;
        } else if (c == ']' && idx < out.length() && out.at(idx) == ']') {
            QString sd;
            if (seed) {
                sd = sofar;
                if (sd.isNull()) sd = "";
            } else {
                cur.push_back(sofar);
                sofar = {};
                opts.push_back(cur);
            }
            return {replfn({opts, sd}), idx+1};
        } else {
            sofar += c;
        }
    }
    return {{}, -1};
}
QString matchHiddens(const QString& out, std::function<QString(hiddenMatch)> replfn) {
    QString end;
    bool ended = false;
    qsizetype idx = 0;
    while (idx < out.length()) {
        QChar c = out.at(idx++);
        if (c == '\\') {
            end += '\\';
            if (idx < out.length()) { end += out.at(idx++); }
        } else if (c == '[' && idx < out.length() && out.at(idx) == '[') {
            const auto o = _matchHiddensInner(idx+1, out, replfn);
            if (o.second == -1) {
                end += c;
                continue;
            }
            end += o.first;
            idx = o.second;
        } else {
            end += c;
        }
    }
    return end;
}

QString HiddenFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME || s == SIDE_GETFC) return txt;
    if (s == SIDE_HIDE) return matchHiddens(txt, [](hiddenMatch){return "";});

    auto rng = getRNG("Hidden");
    return matchHiddens(txt, [s, &rng](hiddenMatch m){
        if (m.opts.size() == 0) return QString();
        if (m.opts.size() == 1 && m.opts[0].size() <= 1) return QString();
        uint idx;
        int mx = m.opts.size();
        if (m.seed.isNull()) {
            idx = rng.bounded(mx);
        } else {
            idx = getRNG("Hidden-"+m.seed).bounded(mx);
        }
        QStringList item = m.opts.at(idx);
        if (item.size() == 0) return QString();
        if (item.size() == 1) return item[0];
        if (s == SIDE_BACK) return item[1];
        return item[0];
    });
}
QString HiddenFeat::markup(QString& line) const {
    const QString b = "<b style='color:" + col + "'>";
    return matchHiddens(line, [&b](hiddenMatch m){
        QStringList outs;
        for (auto& it : m.opts) {
            outs.append(it.join("::"));
        }
        QString out = b+"[[</b>"+outs.join("//");
        if (m.seed.isNull()) return out+b+"]]</b>";
        return out+b+"##</b>"+m.seed+b+"]]</b>";
    });
}
QMap<QString, QString> HiddenFeat::help() const {
    return {
           {"Always hidden text\n[[]]",
            "Always hides text (so can be seen as a 'comment')\n"
            "E.g. [[This text will NEVER be seen and doesn't affect anything!]]\n"
        }, {"Hidden on sides\n[[::]]",
            "Only shows text if it's on a specific side\n"
            "E.g. `[[hi::bye]]` would show `hi` if it's on the front side of a flashcard and `bye` if it's on the back.\n"
            "These can be stacked (though this is more useful with randomly hidden)\n"
            "Leaving sides blank is ok (e.g. `[[front::]]`)"
        }, {"Randomly hidden\n[[//##]]",
            "Randomly selects one of the listed features to use\n"
            "E.g. `[[A//B]]` would show either A or B with a 50% chance\n"
            "This can be combined with hidden on sides, e.g. `[[choice1//choice2front::choice2back//choice3]]`)\n"
            "These can also be stacked e.g. `[[hifr::[[hibk1//hibk2]]//byefr::byebk]]`\n"
            "Using ## will set the seed so you can have the same result for different random choices (e.g. `[[1//2##a]] [[1//2##a]]`)\n"
            "The seed can be any text or number input and MUST BE AT THE END (`[[1##seed//2]]` will use a seed of `seed//2`)."
        }
    };
}

