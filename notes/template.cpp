#include "template.hpp"
#include <QRegularExpression>
#include <QList>

std::map<QString, Template> globalTemplates;

TemplPattern::TemplPattern(QString conts, uint i) {
    name = conts;
    def = "";
    idx = i;
}
QString TemplPattern::apply() {
    return def;
}
QString TemplPattern::apply(QString arg) {
    return arg;
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

const QRegularExpression replRe(R"(%(?<pref>[.^*"]+)?(?<conts>[^|\[.^*"% \n]+)(?<suff>(?:[\[|](?:[^|\[% \n]|\\.)+)+)?(?:%|$|(?=[ \n])))");
QString Template::replace(QStringList args) {
    unsigned int argsln = args.length();
    QString out = conts;

    auto it = replRe.globalMatch(out);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString repl;
        QString conts = m.captured("conts");
        bool ok;
        if (unsigned int num = conts.toUInt(&ok); ok) {
            if (num == 0) continue;
            if (num > argsln) continue;
            repl = args[num-1];
        } else {
            if (ptns.find(conts) != ptns.end()) {
                auto it = ptns.find(conts);
                if (it == ptns.end()) continue;
                auto& p = it->second;
                if (p.idx >= argsln) {
                    repl = p.apply();
                } else {
                    repl = p.apply(args[p.idx]);
                }
            } else {
                continue;
            }
        }
        if (repl.isNull()) continue;
        if (QString suff = m.captured("suff"); !suff.isNull()) {
            QChar apply;
            QString sofar;
            const static std::vector<QChar> suffs = {'[', '\n'};
            bool good = true;
            bool esc = false;
            for (auto& c : suff+'\n') {
                if (c == '\n') esc = false;
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
                            case '\n':
                            case '[': {
                                auto spl = sofar.split(':');
                                auto splln = spl.length();
                                if (splln == 0 || splln > 2) {
                                    good = false;
                                    break;
                                }
                                uint replln = repl.length();
                                bool ok;
                                int from = spl[0].toInt(&ok);
                                if (!ok) {
                                    good = false;
                                    break;
                                }
                                if (from < 0) from = replln + from;
                                if (splln == 1) {
                                    if (from >= replln || from < 0) {
                                        repl = "";
                                        break;
                                    }
                                    repl.slice(from, 1);
                                } else {
                                    bool ok2;
                                    int to = spl[1].toInt(&ok2);
                                    if (!ok2) {
                                        good = false;
                                        break;
                                    }
                                    if (from < 0) from = 0;
                                    if (to < 0) to = replln + to;
                                    if (from > to || from >= replln || to < 0) {
                                        repl = "";
                                        break;
                                    }
                                    if (to >= replln) to = replln-1;
                                    repl.slice(from, to-from+1);
                                }
                                break;}
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
            }
            if (!good) continue;
        }
        if (QString pref = m.captured("pref"); !pref.isNull()) {
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
