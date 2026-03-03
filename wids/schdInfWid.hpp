#pragma once
#include <QWidget>
#include <QLabel>
#include "../notes/note.hpp"

class SchdInfoWid : public QWidget {
public:
    SchdInfoWid(QWidget* parent = nullptr);
    void chngNote(Note* note = nullptr);
protected:
    QLabel* txt;
    ScheduleMap map;
    bool empty;
};
