#include "features.hpp"
#include "template.hpp"
#include <QRegularExpression>
#include <QList>

/* Escape codes
 * \2 - replacement for spaces
 * \3 - represents a \ that only escapes and doesn't have any physical form
 * \6 - used to represent every number separated by spaces
 * \F - used to state this variable should be expanded
 *
 * \A and \D are LF and CR respectively (so do not use them!)
 */

std::map<QString, Template> globalTemplates;

const QString prefs = "\".^*_&";
const QString suffs = "[|{&;+>!=\r";


void Template::handlePtns(QString p) {
    p = p.trimmed();
    if (p.isEmpty()) return;
    // Not indexing idx, but pattern number
    uint idx = 0;
    uint lastidx = 0;
    bool rev = false;
    bool escaped = false;
    QString sofar;
    QStringList out;
    uint stage = 0;
    int indent = 0;
    for (auto& c : p+' ') {
        if (escaped) {
            if (c != '\n') {
                if (c != ' ' && c != ']') sofar += '\\';
                sofar += c;
            }
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (stage == 2 && c == '(') {
            sofar += c;
            indent++;
        } else if (stage == 2 && c == ')') {
            indent--;
            if (indent < 0) return;
            sofar += c;
        } else if (indent > 0) {
            sofar += c;
        } else if (stage == 0 && sofar.isEmpty() && c.isSpace()) {
            // Do nothing
        } else if (c.isSpace()) {
            if (stage == 0) {
                if (sofar == "*") {
                    uint cur = idx;
                    idx = lastidx;
                    lastidx = cur;
                    rev = !rev;
                } else if (sofar == "-") {
                    idx++;
                }
                sofar = {};
                continue;
            }
            out << sofar;
            QString pref = out.value(0, "");
            QString name = out.value(1, "");
            QString suff = out.value(2, "");

            if (!name.isEmpty()) {
                uint suffln;
                if (suff.startsWith('\3')) {
                    suffln = 0;
                    suff = suff.sliced(1);
                } else {
                    suffln = suff.length();
                }
                if (suffln >= 2 && suff[0] == '#' && suff[1] == '=') {
                    ptns.emplace(name, suff.sliced(2));
                } else if (suffln >= 2 && suff[0] == '(' && suff[1] == '=') {
                    ptns.emplace(name, suff.sliced(2, suff.length()-3));
                } else if (suffln >= 2 && suff[0] == '&' && suff[1] == '=') {
                    ptns.emplace(name, "\f%x&"+suff.sliced(2));
                } else if (suffln > 0 && suff[0] == '=') {
                    ptns.emplace(name, '\f'+suff.sliced(1));
                } else {
                    QString minus = rev ? "-" : "";
                    ptns.emplace(name, "\f%"+pref+minus+QString::number(++idx)+suff);
                }
            }

            sofar = {};
            out = {};
            stage = 0;
        } else if (
            (stage == 0 && !prefs.contains(c)) ||
            (stage == 1 && !(c.isLetterOrNumber() || c == '-'))
        ) {
            stage++;
            if (escaped) {
                if (stage == 2) {
                    out << sofar;
                    sofar = '\3'+c;
                } else {
                    out << sofar+'\\';
                    sofar = c;
                }
                escaped = false;
            } else {
                out << sofar;
                sofar = c;
                if (c == '(') indent++;
            }
        } else {
            sofar += c;
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

QString Template::replace(QStringList args) {
    if (failed()) return "==<ERROR>==";
    for (auto& str : args) {
        str = ScheduleFeat::instance->replacements(str, SIDE_HIDE);
    }
    QString out = replace_inner(args, conts, 0);
    return out
        .replace('\2', ' ')
        .replace('\f', "");
}
QString Template::replace() {
    return replace({});
}

const std::vector<QChar> suffsList() {
    std::vector<QChar> vec;
    vec.reserve(suffs.size() + 2);

    vec.insert(vec.end(), suffs.begin(), suffs.end());
    vec.push_back(QChar('\n'));
    return vec;
}

Template::_parseRes Template::parseLoop(QStringList args, qsizetype idx, const QString& out, uint depth) {
    uint stage = 0;
    QString countr;
    QString loopovr;
    QString end;
    while (idx < out.length()) {
        QChar c = out.at(idx++);
        bool esc = c == '\\';
        if (esc) {
            if (idx >= out.length()) break;
            c = out.at(idx++);
            if (c == '\n') continue;
        }

        if (stage == 0) {
            if (c.isSpace()) {
                if (!countr.isEmpty()) stage++;
                continue;
            } else {
                countr += c;
            }
        }
        if (stage == 1) {
            if (!esc && c == '@') {
                stage++;
                continue;
            }
            if (!esc && c == '[') {
                bool good = false;
                QString inner;
                auto idx2 = idx;
                while (idx2 < out.length()) {
                    QChar c = out.at(idx2++);
                    if (c == '\\') {
                        if (idx2 < out.length()) {
                            inner += out.at(idx2++);
                        } else {
                            return {{}, -1};
                        }
                    } else if (c == ']' && (idx2 >= out.length() || out.at(idx2).isSpace())) {
                        while (idx2 < out.length() && out.at(idx2).isSpace()) idx2++;
                        good = true;
                        break;
                    } else {
                        inner += c;
                    }
                }
                if (good) {
                    idx = idx2;
                    handlePtns(inner);
                    continue;
                }
            }
            if (esc) loopovr += '\\';
            loopovr += c;
            continue;
        }
        if (stage == 2) {
            if (!esc && end.isEmpty() && c.isSpace()) continue;
            if (!esc && c == '@' && idx < out.length() && out.at(idx) == '@') {
                QString o;
                QStringList its = replace_inner(args, loopovr.trimmed(), depth+1).split(' ');
                for (auto& it : its) {
                    if (it.isEmpty()) continue;
                    ptns[countr] = it;
                    o += replace_inner(args, end, depth+1);
                }
                return {o, idx+1};
            }
            if (esc) end += '\\';
            end += c;
        }
    }
    return {{}, -1};
}
Template::_parseRes Template::parseVar(QStringList args, qsizetype idx, const QString& out, uint depth) {
    uint stage = 0;
    bool isdollar = out.at(idx++) == '$';
    QString pref;
    QString nam;
    QString suff;
    while (idx < out.length()) {
        QChar c = out.at(idx++);

        if (stage == 0) {
            if (prefs.contains(c)) {
                pref += c;
                continue;
            } else { stage++; }
        }
        if (stage == 1) {
            if (c.isLetterOrNumber() || c == '-' || nam.isEmpty()) {
                nam += c;
                continue;
            } else {
                if (isdollar) {
                    idx--;
                    break;
                }
                stage++;
            }
        }
        if (stage == 2) {
            if (c == '\\') {
                if (idx < out.length()) {
                    QChar newc = out.at(idx++);
                    if (newc == '\n') continue;
                    if (newc == '(') {
                        suff += "(";
                    } else if (newc == ')') {
                        suff += ")";
                    } else {
                        suff += '\\';
                        suff += newc;
                    }
                    continue;
                } else { break; }
            }
            if (c == '(') {
                if (idx >= out.length()) {
                    suff += '(';
                    break;
                }
                QString whole = "(";
                int indent = 1;
                auto idx2 = idx;
                while (indent > 0 && idx2 < out.length()) {
                    QChar c = out.at(idx2++);
                    if (c == '\\') {
                        if (idx2 < out.length()) {
                            QChar newc = out.at(idx2++);
                            if (newc == '\n') continue;
                            if (newc == '(') {
                                whole += "(";
                            } else if (newc == ')') {
                                whole += ")";
                            } else {
                                whole += '\\';
                                whole += newc;
                            }
                            continue;
                        } else { break; }
                    }
                    if (c == ')') indent--;
                    else if (c == '(') indent++;
                    whole += c;
                }
                if (indent <= 0) {
                    idx = idx2;
                    suff += whole;
                    continue;
                }
            }
            if (c == '%') {
                break;
            }
            if (c.isSpace()) {
                idx--;
                break;
            }
            suff += c;
        }
    }
    QString o = parseArg(args, nam, pref, suff, depth+1);
    if (o.isNull()) return {{}, -1};
    return {o, idx};
}

QString Template::replace_inner(QStringList args, QString out, uint depth) {
    if (depth > MAX_RECURSION) return out;
    auto origPtns = ptns;

    QString ret;
    for (qsizetype idx = 0; idx < out.length(); idx++) {
        QChar c = out.at(idx);
        
        if (c == '\\') {
            if (++idx < out.length()) {
                ret += '\\';
                ret += out.at(idx);
            } else {
                ret += c;
            }
            continue;
        }
        if (c == '@' && idx+1 < out.length() && out.at(idx+1) == '@') {
            auto o = parseLoop(args, idx+2, out, depth);
            ptns = origPtns;
            if (o.second != -1) {
                ret += o.first;
                idx = o.second-1;
                continue;
            }
        }
        if (c == '%' || c == '$') {
            auto o = parseVar(args, idx, out, depth);
            if (o.second != -1) {
                ret += o.first;
                idx = o.second-1;
                continue;
            }
        }
        ret += c;
    }
    return ret;
}

QString Template::parseArg(QStringList args, QString nam, QString pref, QString suff, uint depth) {
    if (nam.isEmpty()) return {};

    QString repl;
    if (auto it = ptns.find(nam); it != ptns.end()) {
        repl = it->second;
    } else if (nam == "-") {
        repl = '\6';
    } else if (nam == "+") {
        repl = "\f%0+( | )";
    } else {
        if (nam == "#") {
            nam = "0";
        }
        bool ok;
        if (int num = nam.toInt(&ok); ok) {
            if (num == 0) {
                QStringList as = args;
                for (auto& a : as) {
                    a.replace(' ', '\2');
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
            repl = "";
        }
    }
    if (repl != "\6" && repl.startsWith('\f')) {
        repl = replace_inner(args, repl.sliced(1), depth+1);
    }
    if (repl.isNull()) repl = "";

    if (!suff.isNull()) {
        QChar apply;
        QString sofar;
        const static std::vector<QChar> suffs = suffsList();
        bool esc = false;
        int indent = 0;
        for (auto& c : suff+'\r') {
            if (esc) {
                sofar += c;
                esc = false;
            } else if (c == '\\') {
                esc = true;
            } else if (c == '(') {
                if (++indent >= 2) sofar += c;
            } else if (c == ')') {
                if (indent-- >= 2) sofar += c;
                if (indent < 0) return {};
            } else if (indent > 0) {
                sofar += c;
            } else if (std::find(suffs.begin(), suffs.end(), c) != suffs.end()) {
                if (apply.isNull()) {
                    if (!sofar.isEmpty()) {
                        return {};
                    }
                } else {
                    if (apply != '&')
                        sofar = replace_inner(args, sofar, depth+1);
                    if (sofar.isEmpty() && apply != ';') {
                        return {};
                    }
                    switch (apply.unicode()) {
                        case ';':
                            repl.replace(' ', '\2');
                            sofar.replace(' ', '\2');
                            repl.replace(sofar, " ");
                            if (sofar.isEmpty()) {
                                if (repl.length() <= 1) {
                                    repl = "";
                                } else {
                                    repl = repl.sliced(1, repl.length()-2);
                                }
                            }
                            break;
                        case '+':
                            repl.replace(' ', sofar);
                            break;
                        case '&': {
                            auto oldptns = ptns;
                            ptns["x"] = repl;
                            repl = handlefunc(args, sofar, depth);
                            ptns = oldptns;
                            if (repl.isNull()) repl = "???";
                            break;}
                        case '{':
                        case '[': {
                            bool words = apply == '{';
                            auto spl = sofar.split(':');
                            auto splln = spl.length();
                            if (splln == 0 || splln > 3) {
                                return {};
                            }
                            int replln;
                            if (repl == "\x06") {
                                if (!words) {
                                    return {};
                                }
                                replln = -1;
                            } else {
                                replln = words ? repl.count(' ')+1 : repl.length();
                            }
                            int from;
                            if (spl[0].isEmpty()) {
                                if (splln == 1) {
                                    return {};
                                }
                                from = 0;
                            } else {
                                bool ok;
                                from = spl[0].toInt(&ok);
                                if (!ok) {
                                    repl = "";
                                    break;
                                }
                            }
                            if (from < 0) {
                                if (replln == -1) {
                                    repl = "";
                                    break;
                                }
                                from = replln + from;
                            }
                            if (splln == 1) {
                                if (repl == "\x06") {
                                    repl = QString::number(from);
                                    break;
                                }
                                if (replln != -1 && from >= replln || from < 0) {
                                    repl = "";
                                    break;
                                }
                                if (words) {
                                    repl = repl.split(' ').sliced(from, 1).join(' ');
                                } else {
                                    repl = repl.sliced(from, 1);
                                }
                            } else {
                                int to;
                                if (spl[1].isEmpty()) {
                                    if (replln == -1) {
                                        repl = "";
                                        break;
                                    }
                                    to = replln-1;
                                } else {
                                    bool ok;
                                    to = spl[1].toInt(&ok);
                                    if (!ok) {
                                        repl = "";
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
                                        repl = "";
                                        break;
                                    }
                                }
                                if (step == 0) {
                                    repl = "";
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
                                if (to < 0) {
                                    if (replln == -1) {
                                        repl = "";
                                        break;
                                    }
                                    to = replln + to;
                                }
                                if (from > to || (replln != -1 && from >= replln) || to < 0) {
                                    repl = "";
                                    break;
                                }
                                if (replln != -1 && to >= replln) to = replln-1;
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
                                        for (int i = 0; i < ln; i += step) {
                                            li2.append(li.at(i));
                                        }
                                        li = std::move(li2);
                                    }
                                    if (reverse) std::reverse(li.begin(), li.end());
                                    repl = li.join(' ');
                                } else {
                                    if (step != 1) {
                                        QString out;
                                        for (int i = from; i < to; i += step) {
                                            out += repl.at(i);
                                        }
                                        repl = std::move(out);
                                    } else {
                                        repl = repl.sliced(from, to-from+1);
                                    }
                                    if (reverse) std::reverse(repl.begin(), repl.end());
                                }
                            }
                            if (repl.isNull()) repl = "";
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
                                repl = "";
                            }
                            break;
                        case '!':
                            if (repl != sofar) {
                                repl = "";
                            }
                            break;
                        default:
                            return {};
                    }
                }
                apply = c;
                sofar = "";
            } else {
                sofar += c;
            }
        }
    }
    if (!pref.isNull()) {
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
                case '&':
                    repl = replace_inner(args, repl, depth+1);
                    break;
                default:
                    return {};
            }
        }
    }
    return repl;
}

Template::vartyp Template::hfvar(QStringList args, QString nam, uint depth) {
    QString conts = replace_inner(args, nam, 1);
    bool ok;
    if (float num = conts.toFloat(&ok); ok) {
        return {num};
    }
    return {conts};
}

QSet<QChar> ops = {'+', '-', '*', '/', '~'};
QString Template::handlefunc(QStringList args, QString fn, uint depth) {
    fn = replace_inner(args, fn, depth+1);
    if (fn.isEmpty()) return {};
    QStringList execs;
    QString sofar;
    int indent = 0;
    int curlindent = 0;
    for (auto& c : fn) {
        if (c == '(') {
            if (++indent >= 2) sofar += c;
        } else if (c == ')') {
            if (indent-- >= 2) sofar += c;
            if (indent < 0) return {};
        } else if (indent > 0) {
            sofar += c;
        } else if (c == '{') {
            if (++curlindent < 2) {
                if (!sofar.isEmpty()) {
                    execs << sofar;
                    sofar = {};
                }
            } else { sofar += c; }
        } else if (c == '}') {
            if (curlindent-- >= 2) sofar += c;
            if (curlindent < 0) return {};
            if (curlindent == 0) {
                execs << "%x&"+sofar;
                sofar = {};
            }
        } else if (curlindent > 0) {
            sofar += c;
        } else {
            bool space = c.isSpace();
            if (space || ops.contains(c)) {
                if (!sofar.isEmpty()) {
                    execs << sofar;
                    sofar = {};
                }
                if (!space) execs << c;
            } else {
                sofar += c;
            }
        }
    }
    if (!sofar.isEmpty()) execs << sofar;
    auto exsln = execs.length();
    if (exsln == 0) return {};
    if (exsln == 1) {
        auto out = hfvar(args, execs[0], depth);
        return out.displ();
    }
    if (execs[0] == "-") {
        execs.push_front("0");
        exsln++;
    }
    if (exsln == 2) return {};
    auto out = hfvar(args, execs[0], depth);
    qsizetype idx = 1;
    auto mx = execs.size();
    while (idx < mx) {
        QString op = execs[idx];
        if (op.length() != 1) return {};
        if (++idx >= mx) return {};
        auto with = hfvar(args, execs[idx], depth);
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
                if (!ok) return {};
                float n2 = with.getNum(&ok);
                if (!ok) return {};
                out = {n1-n2};
                break;}
            case '*': {
                bool ok;
                float w = with.getNum(&ok);
                if (!ok) return {};
                if (float n = out.getNum(&ok); ok) {
                    out = {n*w};
                } else {
                    int nw = std::round(w);
                    if (nw < 0) return {};
                    else out = {out.getStr(&ok).repeated(nw)}; // Handles == 0
                }
                break;}
            case '/': {
                bool ok;
                float n1 = out.getNum(&ok);
                if (!ok) return {};
                float n2 = with.getNum(&ok);
                if (n2 == 0 || !ok) return {};
                out = {n1/n2};
                break;}
            case '~': {
                bool ok;
                float n1 = out.getNum(&ok);
                if (!ok) return {};
                float n2 = with.getNum(&ok);
                if (!ok) return {};
                out = {std::fmod(n1, n2)};
                break;}
            default:
                return {};
        }
        idx++;
    }
    return out.displ();
}


struct _ltdRes {
    qsizetype nidx = -1;

    qsizetype startidx = -1;
    qsizetype endidx = -1;
    QString nam = {};
    QString ptn = {};
    QString conts = {};
};

_ltdRes loopTemplDefs(qsizetype idx, const QString& out, bool global) {
    const QChar use = global ? '=':':';
    while (idx < out.length()) {
        QChar c = out.at(idx++);
        if (c == '\\') {
            if (idx < out.length()) { idx++; }
            else { break; }
        } else if (c == '|' && idx < out.length() && out.at(idx) == use) {
            qsizetype start = idx-1;
            idx++;
            uint stage = 0;
            QString nam;
            QString ptn;
            QString conts;
            int indent = 0;
            bool nxtesc = false;
            while (idx < out.length()) {
                QChar c = out.at(idx++);
                bool esc = false;
                if (nxtesc) {
                    nxtesc = false;
                    esc = true;
                } else if (c == '\\') {
                    nxtesc = true;
                }
                if (!esc && stage > 0 && c == '(') {
                    indent++;
                } else if (!esc && indent > 0 && c == ')') {
                    indent--;
                }
                if (!esc && stage > 0 && indent > 0) {
                    esc = true;
                }

                if (!esc && c == use && idx < out.length() && out.at(idx) == '|') {
                    if (ptn.length() > 1) ptn = ptn.sliced(1, ptn.length()-1); // Already removed the last ), should only have the first (
                    else ptn = "";
                    while (start > 0) {
                        if (out.at(start-1).isSpace()) start--;
                        else break;
                    }
                    idx++;
                    auto end = idx;
                    while (idx < out.length()) {
                        QChar c = out.at(idx++);
                        if (c == '\n') end = idx-1;
                        else if (!c.isSpace()) break;
                    }
                    return {end, start, end, nam, ptn, conts.trimmed()};
                }
                if (stage == 0) {
                    if (!esc && c.isSpace()) {
                        if (!nam.isEmpty()) stage++;
                        continue;
                    } else if (!esc && (c == ':' || c == '|')) {
                        stage++;
                        continue;
                    } else { nam += c; }
                } else if (stage == 1) {
                    if (!esc && ptn.isEmpty()) {
                        if (c == '[') ptn += c;
                        else if (!c.isSpace()) {
                            stage++;
                            conts += c;
                        }
                    } else if (!esc && c == ']' && idx < out.length() && (out.at(idx).isSpace() || out.at(idx) == ':' || out.at(idx) == '|')) {
                        stage++;
                    } else {
                        ptn += c;
                    }
                } else {
                    conts += c;
                }
            }
        }
    }
    return {};
}

std::vector<std::pair<QString, Template>> getTempls(QString txt, bool global) {
    std::vector<std::pair<QString, Template>> out;
    qsizetype idx = 0;
    while (1) {
        auto res = loopTemplDefs(idx, txt, global);
        if (res.nidx == -1) break;
        idx = res.nidx;
        out.push_back({res.nam, {res.conts, res.ptn}});
    }
    return out;
}

bool templDefExists(QString txt, bool global) {
    auto res = loopTemplDefs(0, txt, global);
    return res.nidx != -1;
}

QString _rmTemplDefs(QString txt, bool global) {
    qsizetype idx = 0;
    while (1) {
        auto res = loopTemplDefs(idx, txt, global);
        if (res.nidx == -1) break;
        txt = txt.remove(res.startidx, res.endidx - res.startidx);
        if (res.startidx >= txt.length()) break;
        idx = res.startidx;
    }
    return txt;
}
QString rmTemplDefs(QString txt) {
    return _rmTemplDefs(_rmTemplDefs(txt, true), false);
}

_applyRet* applyTempl(QString& txt, QString& err) {
    auto* ret = new _applyRet{0, txt, err};
    applyTempl(ret);
    return ret;
}
std::pair<QString, qsizetype> untilBreak(qsizetype idx, const QString& out) {
    QString end;
    while (idx < out.length()) {
        QChar c = out.at(idx++);
        if (c.isSpace() || c == ':' || c == '|') {
            if (!end.isEmpty()) return {end, idx};
            continue;
        } else {
            end += c;
        }
    }
    return {{}, -1};
}
void applyTempl(_applyRet* inp) {
    QString out = inp->txt;
    qsizetype idx = inp->idx;
    while (idx < out.length()) {
        QChar c = out.at(idx++);
        if (c == '\\') {
            if (idx < out.length()) { idx++; }
            else { break; }
        } else if (c == '|' && idx < out.length() && (out.at(idx) == '|' || out.at(idx) == '!')) {
            QString nam;
            auto idx2 = idx+1;
            if (idx2 >= out.length()) continue;
            if (out.at(idx) == '!') {
                nam = out.at(idx2++);
            } else {
                bool good = false;
                while (idx2 < out.length()) {
                    QChar c = out.at(idx2++);
                    if (c == '\\') {
                        if (idx2 < out.length()) {
                            nam += '\\';
                            nam += out.at(idx2++);
                        }
                        else { break; }
                    } else if (c.isSpace()) {
                        if (!nam.isEmpty()) {
                            good = true;
                            break;
                        }
                        continue;
                    } else if (c == ':' || c == '|') {
                        if (c == '|' && idx2 < out.length() && out.at(idx2) == '|') {
                            idx2--; // So it matches the || after
                        }
                        good = true;
                        break;
                    } else { nam += c; }
                }
                if (!good) continue;
            }
            QString conts;
            while (idx2 < out.length()) {
                QChar c = out.at(idx2++);
                if (c == '|' && idx2 < out.length() && out.at(idx2) == '>' && idx2+1 < out.length() && out.at(idx2+1) == '|') {
                    auto idx3 = idx2+2;
                    QString moreconts = "|>|";
                    bool success = false;
                    while (idx3 < out.length()) {
                        QChar c = out.at(idx3++);
                        if (c == '\\') {
                            if (idx3 < out.length()) {
                                moreconts += '\\';
                                moreconts += out.at(idx3++);
                            }
                            else { break; }
                        } else if (c == '|' && idx3 < out.length() && out.at(idx3) == '<' && idx3+1 < out.length() && out.at(idx3+1) == '|') {
                            conts = conts + moreconts + "|<|";
                            idx2 = idx3+2;
                            success = true;
                            break;
                        } else {
                            moreconts += c;
                        }
                    }
                    if (success) continue;
                } if (c == '\\') {
                    if (idx2 < out.length()) {
                        conts += '\\';
                        conts += out.at(idx2++);
                    }
                    else { break; }
                } else if (c == '|' && idx2 < out.length() && out.at(idx2) == '|') {
                    inp->start = idx-1;
                    inp->end = idx2+1;
                    inp->idx = idx2+1;
                    inp->name = nam;
                    inp->conts = conts.trimmed();
                    return;
                } else { conts += c; }
            }
        }
    }
    inp->idx = -1;
}
