#include "features.hpp"
#include "../log.hpp"
#include <QRegularExpression>

const QString MODULE = "CardFeature";
constexpr auto MO = QRegularExpression::MultilineOption;

QStringList cols = {
    "#E6DDD6",
    "#D6E0D6",
    "#F0D6F0",
    "#D6E0F0",
    "#F0F0D6",
    "#E6FFF0",
    "#E0E0E0",
    "#F0F0F0",
    "#DDE6E6",
    "#E6E6DD",
    "#FDDFDF",
    "#E6DDE6",
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
    REGISTER_CFEAT(SecretFeat);
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
    static const QRegularExpression re(R"(^\s*\n|\n\s*$)");
    return nstr.remove(re);
}


const QRegularExpression ssfRe(R"(^\s*---\s*$)", MO);
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


const QRegularExpression hiddenRe(R"(\[\[((?:.|\n)*?)(?:(?<!\\)::(?<back>(?:.|\n)*?))?\]\])");
QString HiddenFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME) return txt;

    auto it = hiddenRe.globalMatch(txt);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString back = m.captured("back");
        QString repl = "";
        if (!back.isNull()) {
            repl = s == SIDE_BACK ? back : m.captured(1);
        }
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        txt.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return txt;
}
QString HiddenFeat::markup(QString& line) const {
    auto it = hiddenRe.globalMatch(line);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString front = m.captured(1);
        QString back = m.captured("back");
        QString repl = "<b style='color:" + col + "'>[[</b>" + front;
        if (!back.isNull()) {
            repl += "<b style='color:" + col + "'>::</b>" + back;
        }
        repl += "<b style='color:" + col + "'>]]</b>";
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        line.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return line;
}
QMap<QString, QString> HiddenFeat::help() const {
    return {
           {"Always hidden text\n[[]]",
            "Always hides text (so can be seen as a 'comment')\n"
            "E.g. [[This text will NEVER be seen and doesn't affect anything!]]\n"
            "Note features will still do stuff there though (e.g. if you had note templates or something it will still evaluate them)"
        }, {"Hidden on sides\n[[::]]",
            "Only shows text if it's on a specific side\n"
            "E.g. `[[hi::bye]]` would show `hi` if it's on the front side of a flashcard and `bye` if it's on the back.\n"
            "Leaving sides blank is ok (e.g. `[[front::]]`)"
        }
    };
}

