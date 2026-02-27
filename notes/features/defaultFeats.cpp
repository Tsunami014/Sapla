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
        return QString("<span style='r:%1'>═══</span>").arg(col);
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
