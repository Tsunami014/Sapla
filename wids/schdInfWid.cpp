#include "schdInfWid.hpp"
#include <QHBoxLayout>

SchdInfoWid::SchdInfoWid(QWidget* parent) : QWidget(parent) {
    auto lay = new QHBoxLayout(this);

    txt = new QLabel(this);
    txt->setAlignment(Qt::AlignHCenter);
    lay->addWidget(txt);

    lay->addStretch();

    setLayout(lay);
    chngNote(nullptr);
}

void SchdInfoWid::chngNote(Note* note) {
    empty = note == nullptr;
    map = ScheduleMap();
    if (empty) {
        txt->setText("-");
    } else {
        auto nmap = note->getSchdMap();
        unsigned int amnt = 0;
        // Remove empty maps & count them all for the text
        for (const auto& [key, innerMap] : nmap) {
            amnt += innerMap.size();
            if (!innerMap.empty()) {
                map.emplace(key, innerMap);
            }
        }
        txt->setText(
            QString::number(amnt) + " schedules  "
        );
    }
}
