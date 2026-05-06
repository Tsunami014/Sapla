#include "features.hpp"
#include "template.hpp"
#include <QRegularExpression>
#include <QList>

/* Escape codes
 * \3 - replacement for spaces
 * \4 - used as a substitution for %
 * \5 - used as a premature ending for %args
 * \6 - used to represent every number separated by spaces
 * \7 - used to represent an escaped )
 * \9 \B \C - used for escaping base64
 * \E - used as an ending for b64 strings
 *
 * \A and \D are LF and CR respectively
 */

std::map<QString, Template> globalTemplates;

const static QRegularExpression escapeRe(R"((?<!\\)\(((?:\\.|[^)])+)\))");
QString escape(QString inp) {
    QString out = inp;

    auto it = escapeRe.globalMatch(out);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString encoded = m.captured(1).toUtf8().toBase64();
        QString repl = "("+
            encoded.replace('+', '\x09').replace('/', '\x0B').replace('=', '\x0C')
            +"\x0E)";

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        out.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return out;
}
QString deescape(QString inp, bool keepbrackets) {
    QString out = inp;

    auto it = escapeRe.globalMatch(out);
    int offs = 0;
    uint idx = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString cap = m.captured(1);
        QString repl;
        if (cap.endsWith('\x0E')) {
            QByteArray b64 = cap.sliced(0, cap.length()-1).toLatin1()
                .replace('\x09', '+').replace('\x0B', '/').replace('\x0C', '=');
            repl = QString::fromUtf8(QByteArray::fromBase64(b64));
        } else {
            repl = cap;
        }
        if (keepbrackets) {
            repl = "("+repl.replace(')', '\7')+")";
        }

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        out.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    if (!keepbrackets) return out;
    return out.replace('\4', '%').replace('\7', ')');
}

const QRegularExpression hideEscapesRe(R"((?<!\\)\(.*?[^\\)]\))");
QString hideEscapes(QString inp) {
    QString inp2 = inp;
    auto it = hideEscapesRe.globalMatch(inp2);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        auto repl = m.captured().replace('%', '\4');

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        inp2.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return inp2;
}

void Template::GeneratePatterns(QString conts, uint& i, bool rev) {
    QString name;
    QString repl;
    bool usedi = false;
    const QRegularExpression re("([a-zA-Z0-9]+)(.*)");
    for (auto sect : (conts).split(QRegularExpression(R"((?<!\\)(?=/))"))) {
        QString realsect = deescape(sect, true);
        auto m = re.match(realsect);
        if (!m.hasMatch()) continue;
        QString name = m.captured(1);
        QString repl = m.captured(2);
        if (!repl.isEmpty() && repl[0] == '=') {
            ptns.emplace(name, repl.sliced(1));
        } else {
            QString pref = rev ? "-" : "";
            ptns.emplace(name, '%'+pref+QString::number(i+1)+repl);
            usedi = true;
        }
    }
    if (usedi) i++;
}
void Template::handlePtns(QString p) {
    if (p.isEmpty()) return;
    uint idx = 0;
    uint lastidx = 0;
    bool rev = false;
    for (auto sub : escape(p).split(QRegularExpression(R"((?<!\\)[ \n])"))) {
        if (sub != "") {
            if (sub == "-") {
                idx++;
            } else if (sub == "*") {
                uint cur = idx;
                idx = lastidx;
                lastidx = cur;
                rev = !rev;
            } else {
                GeneratePatterns(sub, idx, rev);
            }
        }
    }
}

Template::Template(QString c, QString p) {
    conts = c.isNull() ? "" : c;
    ptns = {};
    handlePtns(p);
}
Template::Template() {
    conts = QString();
    ptns = {};
}

bool Template::failed() {
    return conts.isNull();
}

const QRegularExpression loopRe(
    R"((?:(?<=\n)\s*|[ \t]*)@@\s*(?<nam>[a-zA-Z0-9]+)\s+(?:\[(?<spec>(?:(?<!\\)\(.*?[^\\)]\)|\\\]|[^\]])*)\]\s+)?(?<its>(?:\((?:\\\)|[^)])*\)|\\@|[^@])+)\s*@\s*(?<conts>(?:\n|.)+?)@@(?:\s*(?=\n)|[ \t]*))");
const QString prefs = "\".^*_";
const QString suffs = "\\[|{&;+>!=\r";
const QRegularExpression replRe(
    "(?<!%|\\\\)%(?<pref>["+prefs+"]+)?"
    "(?<conts>[a-zA-Z0-9\\-]+)"
    "(?<suff>(?:["+suffs+R"(](?:(?<!\\)\([^\x05\n]*?[^\\)]\)|\\[^\x05\n]|[^\x05% \n)"+suffs+"])*)+)?"
    "(?:\x05|%|$|(?=[ \n$]))"

  R"(|(?<!\$)\$)"
    "(?<pref2>["+prefs+"]+)?"
    "(?<conts2>[a-zA-Z0-9\\-]+)");
const std::vector<QChar> suffsList() {
    std::vector<QChar> vec;
    vec.reserve(suffs.size() + 2);

    vec.insert(vec.end(), suffs.begin(), suffs.end());
    vec.push_back(QChar('\n'));
    return vec;
}
QString Template::replace_inner(QStringList args, QString out, uint depth) {
    {
        auto it = loopRe.globalMatch(out);
        int offs = 0;
        while (it.hasNext()) {
            auto m = it.next();
            QString repl;

            QString nam = m.captured("nam");
            QString inner = m.captured("conts");
            auto oldptns = ptns;
            handlePtns(m.captured("spec"));
            QStringList items = replace_inner(args, m.captured("its"), 1).split(' ');
            for (auto& it : items) {
                if (it.isEmpty()) continue;
                ptns[nam] = it;
                repl += replace_inner(args, inner, 1);
            }
            ptns = oldptns;

            int start = m.capturedStart(0) + offs;
            int end = m.capturedEnd(0) + offs;
            out.replace(start, end - start, repl);
            offs += repl.length() - (end - start);
        }
    }

    auto it = replRe.globalMatch(hideEscapes(out));
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString repl;
        QString conts = m.captured("conts");
        if (conts.isNull()) conts = m.captured("conts2");
        if (auto it = ptns.find(conts); it != ptns.end()) {
            if (depth+1 > MAX_RECURSION) continue;
            repl = replace_inner(args, it->second, depth+1);
        } else if (conts == "-") {
            repl = '\x06';
        } else {
            if (conts == "#") {
                conts = "0";
            }
            bool ok;
            if (int num = conts.toInt(&ok); ok) {
                if (num == 0) {
                    QStringList as = args;
                    for (auto& a : as) {
                        a.replace(' ', '\3');
                    }
                    repl = as.join(' ');
                } else {
                    if (num <= 0) {
                        num = args.length()+num + 1;
                    }
                    if (num > 0 && num <= args.length()) {
                        repl = args[num-1];
                    }
                }
            } else {
                continue;
            }
        }

        QString pref = m.captured("pref");
        if (pref.isNull()) pref = m.captured("pref2");
        if (!parseArg(args, repl, pref, escape(m.captured("suff")))) continue;
        if (repl.contains('\x06')) continue;
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        out.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    if (depth != 0) return out;
    return out.replace("%%", "%").replace('\3', ' ').replace('\5', "").replace('\4', '%');
}
QString Template::replace_main(QStringList args) {
    if (failed()) return "==<ERROR>==";
    for (auto& str : args) {
        str = ScheduleFeat::instance->replacements(str, SIDE_HIDE);
    }
    return replace_inner(args, conts, 0);
}
QString Template::replace(QStringList args) {
    return replace_main(args);
}
QString Template::replace() {
    return replace_main({});
}

bool Template::parseArg(QStringList args, QString& repl, QString pref, QString suff) {
    if (!suff.isNull()) {
        QChar apply;
        QString sofar;
        const static std::vector<QChar> suffs = suffsList();
        bool good = true;
        bool esc = false;
        for (auto& c : suff+'\r') {
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
                    if (apply != '&')
                        sofar = replace_inner(args, deescape(sofar, true), 1);
                    if (sofar == "" && apply != ';') {
                        good = false;
                        break;
                    }
                    switch (apply.unicode()) {
                        case ';':
                            repl.replace(' ', '\3');
                            sofar.replace(' ', '\3');
                            repl.replace(sofar, " ");
                            if (sofar == "") {
                                repl.slice(1, repl.length()-2);
                            }
                            break;
                        case '+':
                            repl.replace(' ', sofar);
                            break;
                        case '&': {
                            auto oldptns = ptns;
                            ptns["x"] = repl;
                            repl = handlefunc(args, deescape(sofar, false));
                            ptns = oldptns;
                            if (repl.isEmpty()) repl = "???";
                            break;}
                        case '{':
                        case '[': {
                            bool words = apply == '{';
                            auto spl = sofar.split(':');
                            auto splln = spl.length();
                            if (splln == 0 || splln > 3) {
                                good = false;
                                break;
                            }
                            uint replln;
                            if (repl == "\x06") {
                                if (!words) {
                                    good = false;
                                    break;
                                }
                                replln = std::numeric_limits<uint>::max();
                            } else {
                                replln = words ? repl.count(' ')+1 : repl.length();
                            }
                            int from;
                            if (spl[0].isEmpty()) {
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
                                if (repl == "\x06") {
                                    repl = QString::number(from);
                                    break;
                                }
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
                                if (spl[1].isEmpty()) {
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
                                if (splln == 2 || spl[2].isEmpty()) {
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
                                    from = toOld;
                                }
                                if (from < 0) from = 0;
                                if (to < 0) to = replln + to - 1;
                                if (from > to || from >= replln || to < 0) {
                                    repl = {};
                                    break;
                                }
                                if (to >= replln) to = replln-1;
                                if (repl == "\x06") {
                                    QStringList li = {};
                                    for (int i = from; i < to; i += step) {
                                        li.append(QString::number(i));
                                    }
                                    if (reverse) std::reverse(li.begin(), li.end());
                                    repl = li.join(' ');
                                } else if (words) {
                                    int ln = to-from+1;
                                    auto li = repl.split(' ').sliced(from, ln);
                                    if (step != 1) {
                                        QStringList li2 = {};
                                        for (uint i = 0; i < ln; i += step) {
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
                                        repl = repl.slice(from, to-from+1);
                                    }
                                    if (reverse) std::reverse(repl.begin(), repl.end());
                                }
                            }
                            break;}
                        case '|':
                            if (repl.isEmpty())
                                repl = sofar;
                            break;
                        case '>':
                            if (!repl.isEmpty())
                                repl = sofar;
                            break;
                        case '=':
                            if (repl == sofar) {
                                repl = {};
                            }
                            break;
                        case '!':
                            if (repl != sofar) {
                                repl = {};
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
        }
        if (!good) return false;
    }
    if (!pref.isNull()) {
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
                case '_':
                    repl = QString::number(repl.count(' ')+1);
                    break;
                default:
                    good = false;
                    break;
            }
            if (!good) break;
        }
        if (!good) return false;
    }
    return true;
}

QSet<QChar> spls = {' ', '+', '-', '*', '/', '~'};
Template::vartyp Template::hfvar(QStringList args, QString nam) {
    QString conts = replace_inner(args, deescape(nam, true), 1);
    bool ok;
    if (float num = conts.toFloat(&ok); ok) {
        return {num};
    }
    return {conts};
}
QString Template::handlefunc(QStringList args, QString fn) {
    QStringList execs;
    QString part;
    uint indent = 0;
    bool escaped = false;
    bool escaping = false;
    for (auto& c : fn) {
        if (escaped) {
            escaped = false;
            if (escaping) part += "\\";
            part += c;
            continue;
        } if (c == '\\') {
            escaped = true;
            continue;
        } if (c == '|') {
            if (!part.isEmpty()) {
                execs.push_back(part);
                part = {};
            }
            escaping = !escaping;
            continue;
        } if (escaping) {
            part += c;
            continue;
        } if (c == '}') {
            if (indent == 0) return "";
            indent--;
            if (indent == 0) {
                QString handl = handlefunc(args, part);
                if (handl.isEmpty()) return "";
                execs.push_back(handl);
                continue;
            }
        } if (c == '{') {
            if (indent == 0 && !part.isEmpty()) {
                execs.push_back(part);
                part = {};
            } else if (indent > 0) {
                part += c;
            }
            indent++;
            continue;
        }
        if (indent != 0) {
            part += c;
            continue;
        }
        if (spls.find(c) != spls.end()) {
            if (!part.isEmpty())
                execs.push_back(part);
            if (c != ' ') {
                execs.push_back(c);
            }
            part = {};
            continue;
        }
        part += c;
    }
    if (indent != 0) return "";
    if (!part.isEmpty()) execs.push_back(part);
    uint exsln = execs.length();
    if (exsln == 0) return "";
    if (exsln == 1) {
        auto out = hfvar(args, execs[0]);
        return out.displ();
    }
    if (execs[0] == "-") {
        execs.push_front("0");
        exsln++;
    }
    if (exsln == 2) return "";
    auto out = hfvar(args, execs[0]);
    uint idx = 1;
    uint mx = execs.size();
    while (idx < mx) {
        QString op = execs[idx];
        if (op.length() != 1) return "";
        if (++idx >= mx) return "";
        auto with = hfvar(args, execs[idx]);
        switch (op[0].unicode()) {
            case '+': {
                bool ok;
                float n1 = out.getNum(&ok);
                if (ok) {
                    float n2 = with.getNum(&ok);
                    if (ok) { out = {n1+n2}; break; }
                }
                out = {out.displ()+with.displ()};
                break;}
            case '-': {
                bool ok;
                float n1 = out.getNum(&ok);
                if (!ok) return "";
                float n2 = with.getNum(&ok);
                if (!ok) return "";
                out = {n1-n2};
                break;}
            case '*': {
                bool ok;
                float w = with.getNum(&ok);
                if (!ok) return "";
                if (float n = out.getNum(&ok); ok) {
                    out = {n*w};
                } else {
                    int nw = std::round(w);
                    if (nw < 0) return "";
                    else out = {out.getStr(&ok).repeated(nw)}; // Handles == 0
                }
                break;}
            case '/': {
                bool ok;
                float n1 = out.getNum(&ok);
                if (!ok) return "";
                float n2 = with.getNum(&ok);
                if (n2 == 0 || !ok) return "";
                out = {n1/n2};
                break;}
            case '~': {
                bool ok;
                float n1 = out.getNum(&ok);
                if (!ok) return "";
                float n2 = with.getNum(&ok);
                if (!ok) return "";
                out = {std::fmod(n1, n2)};
                break;}
            default:
                return "";
        }
        idx++;
    }
    return out.displ();
}
