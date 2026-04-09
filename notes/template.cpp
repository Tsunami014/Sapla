#include "template.hpp"
#include <QRegularExpression>
#include <QList>

std::map<QString, Template> globalTemplates;

TemplPattern::TemplPattern(QString conts, uint i) {
    const static auto splby = QRegularExpression(R"((?<!\\)\|)");

    auto spl = conts.split(splby);
    name = spl.front();
    spl.pop_front();
    def = spl.join('|');
    idx = i;
}

Template::Template(QString c, QString p) {
    conts = c.isNull() ? "" : c;
    ptns = {};
    if (p.isNull()) return;
    uint idx = 0;
    for (auto sub : p.split(" ")) {
        if (sub != "") {
            auto p = TemplPattern(sub, idx++);
            ptns.emplace(p.name, std::move(p));
        }
    }
}

const QRegularExpression replRe(
    R"(%(?<pref>[.^*\"]+)?)"
    R"((?<conts>(?:\\[^\n\r]|[^|\[{.^*"% \n\r])+))"
    R"((?<suff>(?:[\[|{](?:\\[^\n\r]|[^|\[{% \n\r])+)+)?)"
    R"((?:%|$|(?=[ \n\r])))");
QString Template::replace(QStringList args) {
    unsigned int argsln = args.length();
    QString out = conts;

    auto it = replRe.globalMatch(out);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString repl;
        QString def;
        QString conts = m.captured("conts").replace('\\', "");
        bool ok;
        if (unsigned int num = conts.toUInt(&ok); ok) {
            if (num == 0) continue;
            if (num <= argsln) {
                repl = args[num-1];
            }
            def = "";
        } else {
            if (ptns.find(conts) != ptns.end()) {
                auto it = ptns.find(conts);
                if (it == ptns.end()) continue;
                auto& p = it->second;
                if (p.idx < argsln) {
                    repl = args[p.idx];
                }
                def = p.def;
            } else {
                continue;
            }
        }
        QString suff = m.captured("suff").replace('\\', "");
        if (suff.isEmpty()) {
            suff = '\n'+def+'\r';
        } else if (suff[0] == '|') {
            suff += '\r';
        } else {
            suff = '\n'+def+'\r'+suff+'\r';
        }
        {
            QChar apply;
            QString sofar;
            const static std::vector<QChar> suffs = {'|', '[', '{', '\r', '\n'};
            bool good = true;
            bool esc = false;
            for (auto& c : suff) {
                if (c == '\n' || c == '\r') esc = false;
                if (esc) {
                    sofar += c;
                    esc = false;
                } else if (c == '\\') {
                    esc = true;
                } else if (std::find(suffs.begin(), suffs.end(), c) != suffs.end()) {
                    if (apply.isNull()) {
                        if (!sofar.isEmpty()) {
                            good = false;
                            break;
                        }
                    } else {
                        switch (apply.unicode()) {
                            case '{':
                            case '[': {
                                bool words = apply == '{';
                                auto spl = sofar.split(':');
                                auto splln = spl.length();
                                if (splln == 0 || splln > 3) {
                                    good = false;
                                    break;
                                }
                                uint replln = words ? repl.count(' ')+1 : repl.length();
                                int from;
                                if (spl[0] == "") {
                                    if (splln == 1) {
                                        good = false;
                                        break;
                                    }
                                    from = 0;
                                } else {
                                    bool ok;
                                    from = spl[0].toInt(&ok);
                                    if (!ok) {
                                        good = false;
                                        break;
                                    }
                                }
                                if (from < 0) from = replln + from - 1;
                                if (splln == 1) {
                                    if (from >= replln || from < 0) {
                                        repl = {};
                                        break;
                                    }
                                    if (words) {
                                        repl = repl.split(' ').sliced(from, 1).join(' ');
                                    } else {
                                        repl.slice(from, 1);
                                    }
                                } else {
                                    bool ok2;
                                    int to;
                                    if (spl[1] == "") {
                                        to = replln-1;
                                    } else {
                                        bool ok;
                                        to = spl[1].toInt(&ok);
                                        if (!ok) {
                                            good = false;
                                            break;
                                        }
                                    }
                                    int step;
                                    if (splln == 2 || spl[2] == "") {
                                        step = 1;
                                    } else {
                                        bool ok;
                                        step = spl[2].toInt(&ok);
                                        if (!ok) {
                                            good = false;
                                            break;
                                        }
                                    }
                                    if (step == 0) {
                                        good = false;
                                        break;
                                    }
                                    bool reverse = step < 0;
                                    if (reverse) {
                                        step = -step;
                                        int toOld = to;
                                        to = from;
                                        from = to;
                                    }
                                    if (from < 0) from = 0;
                                    if (to < 0) to = replln + to - 1;
                                    if (from > to || from >= replln || to < 0) {
                                        repl = {};
                                        break;
                                    }
                                    if (to >= replln) to = replln-1;
                                    if (words) {
                                        int ln = to-from+1;
                                        auto li = repl.split(' ').sliced(from, ln);
                                        if (step != 1) {
                                            QStringList li2 = {};
                                            for (uint i = 0; i <= ln; i += step) {
                                                li2.append(li[i]);
                                            }
                                            li = std::move(li2);
                                        }
                                        if (reverse) std::reverse(li.begin(), li.end());
                                        repl = li.join(' ');
                                    } else {
                                        if (step != 1) {
                                            QString out;
                                            for (uint i = from; i <= to; i += step) {
                                                out += repl[i];
                                            }
                                            repl = std::move(out);
                                        } else {
                                            repl.slice(from, to-from+1);
                                        }
                                        if (reverse) std::reverse(repl.begin(), repl.end());
                                    }
                                }
                                break;}
                            case '\n':
                            case '|':
                                if (repl.isNull()) {
                                    repl = sofar;
                                }
                                break;
                            default:
                                good = false;
                                break;
                        }
                    }
                    apply = c;
                    sofar = "";
                } else {
                    sofar += c;
                }
                if (!good) break;
                if (c == '\r') {
                    apply = {};
                }
            }
            if (!good) continue;
        }
        if (QString pref = m.captured("pref").replace('\\', ""); !pref.isNull()) {
            bool good = true;
            for (auto& c : pref) {
                switch (c.unicode()) {
                    case '.':
                        repl = repl.toLower();
                        break;
                    case '*':
                        repl = repl.toUpper();
                        break;
                    case '^': {
                        QStringList words = repl.split(' ');
                        for (QString &w : words) {
                            if (!w.isEmpty())
                                w[0] = w[0].toUpper();
                        }
                        repl = words.join(' ');
                        break;}
                    case '"': {
                        uint cap = 2;
                        for (int i = 0; i < repl.size(); ++i) {
                            QChar &c = repl[i];
                            if (cap != 0 && c.isLetterOrNumber()) {
                                if (cap == 2) c = c.toUpper();
                                cap = 0;
                            } else if (c == '.' || c == '!' || c == '?') {
                                cap = 1;
                            } else if (cap == 1 && c.isSpace()) {
                                cap = 2;
                            }
                        }
                        break;}
                    default:
                        good = false;
                        break;
                }
                if (!good) break;
            }
            if (!good) continue;
        }

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        out.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return out.replace("%%", "%");
}
QString Template::replace() {
    return replace({});
}
