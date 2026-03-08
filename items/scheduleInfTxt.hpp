#pragma once
#include "../notes/note.hpp"
#include "../base/rectIt.hpp"
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QWidget>

class ScheduleInfoTxt : public RectItem {
public:
    ScheduleInfoTxt(QGraphicsItem* parent = nullptr);
    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {}

    void generate(FlashCard* card);
    void remove();
protected:
    QGraphicsProxyWidget proxy;
    void pressKey(int key);

private:
    FlashCard* last;
};
