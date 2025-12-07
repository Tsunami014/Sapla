#pragma once
#include "../notes/note.hpp"
#include <QGraphicsTextItem>

QGraphicsTextItem* getScheduleInfTxt(FlashCard* card, QGraphicsItem* parent = nullptr);
void styleScheduleInfTxt(QGraphicsTextItem* it);
void setScheduleInfTxt(QGraphicsTextItem* it, FlashCard* card);
