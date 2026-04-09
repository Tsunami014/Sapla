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
            continue;
        }
        if (QString pref = m.captured("pref"); !pref.isNull()) {
            std::vector<QChar> done;
            bool good = true;
            for (auto& c : pref) {
                if (std::find(done.begin(), done.end(), c) == done.end()) {
                    done.push_back(c);
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
                } else {
                    good = false;
                    break;
                }
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
