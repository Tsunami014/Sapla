#include "template.hpp"
#include <QRegularExpression>
#include <QList>

std::map<QString, Template> globalTemplates;

const static QRegularExpression escapeRe(R"((?<!\\)\(((?:\\.|[^)])+)\))");
QString escape(QString inp) {
    QString out = inp;

    auto it = escapeRe.globalMatch(out);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QByteArray b64 = m.captured(1).toUtf8().toBase64();
        QString encoded = QString::fromLatin1(b64);
        QString repl = "("+
            encoded.replace('+', '\x0A').replace('/', '\x0B').replace('=', '\x0C')
            +")";

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        out.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return out;
}
QString deescape(QString inp) {
    QString out = inp;

    auto it = escapeRe.globalMatch(out);
    int offs = 0;
    uint idx = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QByteArray b64 = m.captured(1).toLatin1()
            .replace('\x0A', '+').replace('\x0B', '/').replace('\x0C', '=');
        QString repl = QString::fromUtf8(QByteArray::fromBase64(b64));

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        out.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return out;
}

void GeneratePatterns(QString conts, uint& i, std::map<QString, QString>& ptns) {
    if (conts == "-") {
        i++;
        return;
    }
    const static auto splby = QRegularExpression(R"((?<!\\)(?=/))");
    QString name;
    QString repl;
    bool usedi = false;
    for (auto sect : (conts).split(splby)) {
        const static auto re = QRegularExpression("([a-zA-Z0-9]+)(.*)");
        QString realsect = deescape(sect);
        auto m = re.match(realsect);
        if (!m.hasMatch()) continue;
        QString name = m.captured(1);
        QString repl = m.captured(2);
        if (!repl.isEmpty() && repl[0] == '=') {
            ptns.emplace(name, repl.sliced(1));
        } else {
            ptns.emplace(name, '%'+QString::number(i+1)+repl);
            usedi = true;
        }
    }
    if (usedi) i++;
}

Template::Template(QString c, QString p) {
    conts = c.isNull() ? "" : c;
    ptns = {};
    if (p.isNull()) return;
    uint idx = 0;
    const static auto splby = QRegularExpression(R"((?<!\\)\s)");
    for (auto sub : escape(p).split(splby)) {
        if (sub != "") {
            GeneratePatterns(sub, idx, ptns);
        }
    }
}
Template::Template() {
    conts = QString();
    ptns = {};
}

bool Template::failed() {
    return conts.isNull();
}

const QString prefs = ".^*\"";
const QString suffs = "\\[|{;=\r";
const QRegularExpression replRe(
    "(?<!%)%(?<pref>["+prefs+"]+)?"
    "(?<conts>[a-zA-Z0-9]+)"
    "(?<suff>(?:["+suffs+R"(](?:(?<!\\)\(.+?\)|\\[^\x05\n]|[^\x05% \n)"+suffs+"])+)+)?"
    "(?:\x05|%|$|(?=[ \n]))"

  R"(|(?<!\$)\$(?<conts2>[a-zA-Z0-9]+))");
const std::vector<QChar> suffsList() {
    std::vector<QChar> vec;
    vec.reserve(suffs.size() + 2);

    vec.insert(vec.end(), suffs.begin(), suffs.end());
    vec.push_back(QChar('\n'));
    return vec;
}
QString Template::replace(QStringList args, QString out, uint depth) {
    if (depth == 0 && failed()) return "==<ERROR>==";

    auto it = replRe.globalMatch(out);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString repl;
        QString conts = m.captured("conts");
        if (conts.isNull()) conts = m.captured("conts2");
        if (auto it = ptns.find(conts); it != ptns.end()) {
            if (depth+1 > MAX_RECURSION) continue;
            repl = replace(args, it->second, depth+1);
        } else {
            if (conts == "#") {
                conts = "0";
            }
            bool ok;
            if (int num = conts.toInt(&ok); ok) {
                if (num == 0) {
                    repl = args.join('|');
                } else {
                    if (num < 0) {
                        num = args.length()+num + 1;
                    }
                    if (num <= args.length()) {
                        repl = args[num-1];
                    }
                }
            } else {
                continue;
            }
        }

        if (!parseArg(repl, m.captured("pref"), escape(m.captured("suff")))) continue;
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        out.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return out.replace("%%", "%").replace('\3', ' ').replace('\5', "");
}
QString Template::replace(QStringList args) {
    return replace(args, conts);
}
QString Template::replace() {
    return replace({}, conts);
}

bool Template::parseArg(QString& repl, QString pref, QString suff) {
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
                    sofar = deescape(sofar);
                    switch (apply.unicode()) {
                        case ';':
                            repl.replace(' ', '\3');
                            sofar.replace(' ', '\3');
                            repl.replace(sofar, " ");
                            break;
                        case '=':
                            repl.replace(' ', sofar);
                            break;
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
