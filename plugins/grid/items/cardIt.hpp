#pragma once
#include "../game.hpp"

class GridCGI : public CardGraphicItem {
public:
    GridCGI(const QString& fname, const FlashCard* flashc, QGraphicsItem* parent = nullptr);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w);

private:
    bool ontop;
};
