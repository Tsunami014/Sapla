#include "features.hpp"
#include "../log.hpp"
#include <QRegularExpression>

const QString MODULE = "CardFeature";
constexpr auto MO = QRegularExpression::MultilineOption;

QStringList cols = {
    "#D6E0F0",
    "#E6DDD6",
    "#D6E0D6",
    "#F0D6F0",
    "#F0F0D6",
    "#E6FFF0",
    "#F0F0F0",
    "#E0E0E0",
    "#E6DDE6",
    "#DDE6E6",
    "#E6E6DD",
    "#FDDFDF",
    "#DDE6DD"
};
QString _AutoColour::nxtCol() {
    QString& ret = cols[idx];
    idx = (idx + 1) % cols.size();
    return ret;
}

Schedule getSchd(std::map<int, Schedule> schds, int idx) {
    auto it = schds.find(idx);
    if (it != schds.end()) {
        return schds.at(idx);
    }
    return Schedule(idx);
}

void registerNoteFeatures() {
    REGISTER_CFEAT(SingleSideFeat);
    REGISTER_CFEAT(DoubleSideFeat);
    REGISTER_FEAT(TagFeats);
    REGISTER_FEAT(TemplateFeats);
    REGISTER_FEAT(HiddenFeat);

    std::stable_sort(Feats.begin(), Feats.end(), [](const auto& a, const auto& b) {
        return a->order() > b->order();
    });
    std::stable_sort(CardFeats.begin(), CardFeats.end(), [](const auto& a, const auto& b) {
        return a->order() > b->order();
    });

    for (auto& feat : Feats) {
        feat->init();
    }
}

QString trimNL(const QString& orig) {
    QString nstr = orig;
    static const QRegularExpression re(R"(^\n+|\n+$)");
    return nstr.remove(re);
}


const QRegularExpression ssfRe(R"(^\s*--- *\n*$)", MO);
QString SingleSideFeat::markup(QString& line) const {
    if (ssfRe.match(line).hasMatch()) {
        return QString("<span style='color:%1'>───</span>").arg(col);
    }
    return line;
}
bool SingleSideFeat::dominance(const QString& txt) const {
    return ssfRe.match(txt).hasMatch();
}
 std::vector<std::unique_ptr<FlashCard>>SingleSideFeat::getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const {
    QRegularExpressionMatch m = ssfRe.match(txt);
    if (!m.hasMatch()) {
        return {};
    }
    int idx = m.capturedStart();
    std::vector<std::unique_ptr<FlashCard>> l;
    l.push_back(std::make_unique<FlashCard>(
        parent,
        trimNL(txt.left(idx)), trimNL(txt.mid(idx + m.capturedLength())),
        getName(), getSchd(schds, 0)
    ));
    return l;
}
QMap<QString, QString> SingleSideFeat::help() const {
    return {{"Single sided note\n---",
        "Separates the note into a front and a back.\n"
        "If multiple are present, splits off the first one.\n"
        "This creates a single sided card, where the front is above the line and the back is below."
    }};
}

const QRegularExpression dsfRe(R"(^\s*=== *\n*$)", MO);
QString DoubleSideFeat::markup(QString& line) const {
    if (dsfRe.match(line).hasMatch()) {
        return QString("<span style='r:%1'>═══</span>").arg(col);
    }
    return line;
}
bool DoubleSideFeat::dominance(const QString& txt) const {
    return dsfRe.match(txt).hasMatch();
}
 std::vector<std::unique_ptr<FlashCard>>DoubleSideFeat::getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const {
    QRegularExpressionMatch m = dsfRe.match(txt);
    if (!m.hasMatch()) {
        return {};
    }
    int idx = m.capturedStart();
    QString first = trimNL(txt.left(idx));
    QString second = trimNL(txt.mid(idx + m.capturedLength()));
    std::vector<std::unique_ptr<FlashCard>> l;
    l.push_back(std::make_unique<FlashCard>(
        parent, first, second, getName(), getSchd(schds, 0)
    ));
    l.push_back(std::make_unique<FlashCard>(
        parent, second, first, getName(), getSchd(schds, 1)
    ));
    return l;
}
QMap<QString, QString> DoubleSideFeat::help() const {
    return {{"Double sided note\n===",
        "Separates the note into 2 sides.\n"
        "If multiple are present, splits off the first one.\n"
        "This creates 2 cards: one where the top is on the front and the bottom is on the back, and another the other way around."
    }};
}


const QRegularExpression hiddenRe(R"(\[\[((?:.|\n)*?)(?<!\\):((?:.|\n)*?)\]\])");
QString HiddenFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME) return txt;
    QString repl = s == SIDE_BACK ? "\\2" : "\\1";
    return txt.replace(hiddenRe, repl);
}
QString HiddenFeat::markup(QString& line) const {
    return line.replace(hiddenRe, QString("<b style='color:%1'>[[</b>\\1<span style='color:%1'>:</span>\\2<b style='color:%1'>]]</b>").arg(col));
}
QMap<QString, QString> HiddenFeat::help() const {
    return {{"Hidden sides\n[[:]]",
        "Only shows text if it's on a specific side\n"
        "E.g. `[[hi:bye]]` would show `hi` if it's on the front side of a flashcard and `bye` if it's on the back.\n"
        "Leaving sides blank is ok (e.g. `[[front:]]`)"
    }};
}

