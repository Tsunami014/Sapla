#include "scheduleInfTxt.hpp"
#include "../notes/note.hpp"
#include <QTextDocument>
#include <QTextOption>

std::string format_duration(Duration dur) {
    using namespace std::chrono;

    auto mo = duration_cast<months>(dur);
    dur -= mo;
    auto w = duration_cast<weeks>(dur);
    dur -= w;
    auto d = duration_cast<days>(dur);
    dur -= d;
    auto h = duration_cast<hours>(dur);
    dur -= h;
    auto m = duration_cast<minutes>(dur);
    dur -= m;
    auto s = duration_cast<seconds>(dur);

    std::string result;
    if (mo.count() > 0)result += std::to_string(mo.count())+ "mo ";
    if (w.count() > 0) result += std::to_string(w.count()) + "w ";
    if (d.count() > 0) result += std::to_string(d.count()) + "d ";
    if (h.count() > 0) result += std::to_string(h.count()) + "h ";
    if (m.count() > 0) result += std::to_string(m.count()) + "m ";
    if (s.count() > 0) result += std::to_string(s.count()) + "s ";

    if (result.empty()) result = "0s";

    if (result.back() == ' ') result.pop_back();
    return result;
}

QGraphicsTextItem* getScheduleInfTxt(FlashCard* card, QGraphicsItem* parent) {
    QString txt = "<h2>";

    for (int i = 0; i < ScheduleInfo.ratesLen(); i++) {
        txt += "<b>" + std::to_string(i+1) + "</b>: +" + format_duration(card->schd.getUpdatedTime(i)) + "<br>";
    }
    txt += "<b>-</b>: Redo<br>"
           "<b>=</b>: Skip<br>"
        "</h2>";

    auto* gti = new QGraphicsTextItem(parent);
    gti->setHtml(
        txt
    );
    gti->document()->setDefaultTextOption(
        QTextOption(Qt::AlignCenter)
    );
    gti->setDefaultTextColor(Qt::black);
    return gti;
}
