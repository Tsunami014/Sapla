// Some features that come by default in the app (that are small enough to go in only one file)
#include "features.hpp"

const QRegularExpression ssfRe(R"(^\s*---\s*$)", MO);
bool SingleSideFeat::dominance(const QString& txt) const {
    return ssfRe.match(txt).hasMatch();
}
std::vector<std::unique_ptr<FlashCard>> SingleSideFeat::getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const {
    QRegularExpressionMatch m = ssfRe.match(txt);
    if (!m.hasMatch()) {
        return {};
    }
    int idx = m.capturedStart();
    std::vector<std::unique_ptr<FlashCard>> l;
    l.push_back(std::make_unique<FlashCard>(
        parent,
        txt.left(idx), txt.mid(idx + m.capturedLength()),
        getName(), getSchd(schds, 0)
    ));
    return l;
}
QString SingleSideFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_GETFC) return txt;
    return txt.remove(ssfRe);
}
QString SingleSideFeat::markup(QString& line) const {
    if (ssfRe.match(line).hasMatch()) {
        return QString("<span style='color:%1'>───</span>").arg(col);
    }
    return line;
}
QMap<QString, QString> SingleSideFeat::help() const {
    return {{"Single sided note\n---",
        "Separates the note into a front and a back.\n"
        "If multiple are present, splits off the first one.\n"
        "This creates a single sided card, where the front is above the line and the back is below."
    }};
}

const QRegularExpression dsfRe(R"(^\s*===\s*$)", MO);
bool DoubleSideFeat::dominance(const QString& txt) const {
    return dsfRe.match(txt).hasMatch();
}
std::vector<std::unique_ptr<FlashCard>> DoubleSideFeat::getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const {
    QRegularExpressionMatch m = dsfRe.match(txt);
    if (!m.hasMatch()) {
        return {};
    }
    int idx = m.capturedStart();
    QString first = txt.left(idx);
    QString second = txt.mid(idx + m.capturedLength());
    std::vector<std::unique_ptr<FlashCard>> l;
    l.push_back(std::make_unique<FlashCard>(
        parent, first, second, getName(), getSchd(schds, 0)
    ));
    l.push_back(std::make_unique<FlashCard>(
        parent, second, first, getName(), getSchd(schds, 1)
    ));
    return l;
}
QString DoubleSideFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_GETFC) return txt;
    return txt.remove(dsfRe);
}
QString DoubleSideFeat::markup(QString& line) const {
    if (dsfRe.match(line).hasMatch()) {
        return QString("<span style='color:%1'>═══</span>").arg(col);
    }
    return line;
}
QMap<QString, QString> DoubleSideFeat::help() const {
    return {{"Double sided note\n===",
        "Separates the note into 2 sides.\n"
        "If multiple are present, splits off the first one.\n"
        "This creates 2 cards: one where the top is on the front and the bottom is on the back, and another the other way around."
    }};
}

const QRegularExpression msfRe(R"(^\s*\/\/\/\s*$)", MO);
bool MirrorSideFeat::dominance(const QString& txt) const {
    return msfRe.match(txt).hasMatch();
}
std::vector<std::unique_ptr<FlashCard>> MirrorSideFeat::getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const {
    QStringList cards = txt.split(msfRe);
    auto len = cards.length();
    if (len < 2) {
        return {};
    }
    std::vector<std::unique_ptr<FlashCard>> l;
    for (uint i = 0; i < len; i++) {
        QString c = trimNL(cards[i]);
        if (c != "") {
            l.push_back(std::make_unique<FlashCard>(
                parent, c, c, getName(), getSchd(schds, i)
            ));
        }
    }
    return l;
}
QString MirrorSideFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_GETFC) return txt;
    return txt.remove(msfRe);
}
QString MirrorSideFeat::markup(QString& line) const {
    if (msfRe.match(line).hasMatch()) {
        return QString("<span style='color:%1'>╱╱╱</span>").arg(col);
    }
    return line;
}
QMap<QString, QString> MirrorSideFeat::help() const {
    return {{"Mirror sided note\n///",
        "Separates the note into any amount of cards.\n"
        "Each section of the note (separated by ///) is its own card, and the contents will be used for both the front and back."
    }};
}

const QRegularExpression tsfRe(R"(^\s*\|\|\|[ \t]*(.*?)\s*$)", MO);
const QRegularExpression tsfRe2(R"(^\s*\|\|\|[ \t]*(?=(.+?)\s*$))", MO);
QString TemplateSideFeat::check(QString& txt, QString& err) const {
    auto spl = txt.split(tsfRe2);
    if (spl.length() == 1) return txt;
    QStringList parts;
    QStringList titles;

    uint end = 0;
    auto it = tsfRe2.globalMatch(txt);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        parts << txt.mid(end, m.capturedStart() - end);
        titles << m.captured(1);
        end = m.capturedEnd();
    }
    parts << txt.mid(end);

    QString title;
    for (auto t : titles) {
        if (!t.isEmpty()) {
            if (title.isNull()) {
                title = t;
            } else {
                err += "Multiple template titles!\n";
                return "";
            }
        }
    }
    if (title.isNull()) {
        err += "No template titles!\n";
        return "";
    }

    if (auto it = globalTemplates.find(title); it != globalTemplates.end()) {
        auto templ = it->second;
        if (templ.failed()) {
            err += "Template `" + title + "` failed!\n";
            return "";
        }
        return templ.replace(parts);
    }
    err += "Could not find a global template called `"+title+"`!\n";
    return "";
}
QString TemplateSideFeat::replacements(QString& txt, Side s) const {
    if (tsfRe.match(txt).hasMatch())
        return "";
    return txt;
}
QString TemplateSideFeat::markup(QString& line) const {
    if (auto m = tsfRe.match(line); m.hasMatch()) {
        return QString("<span style='color:%1'>│││ %2</span>")
            .arg(col).arg(m.captured(1));
    }
    return line;
}
QMap<QString, QString> TemplateSideFeat::help() const {
    return {{"Template sided note\n|||",
        "Separates the note into parts, then each part is an argument into a note template.\n"
        "Only one ||| must have a name after it, the rest must all have nothing afterwards."
    }};
}
