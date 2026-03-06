#pragma once
#include "../base/rectIt.hpp"
#include "multiprogress.hpp"
#include <QGraphicsTextItem>

class DeckProgress : public RectItem {
public:
    DeckProgress(QGraphicsItem* parent = nullptr);
    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {}
    void upd();
protected:
    void initialupd();
    MultiProgressBarItem pb;
    QGraphicsTextItem txt;
};
