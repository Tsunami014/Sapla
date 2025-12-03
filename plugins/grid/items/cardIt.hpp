#pragma once
#include "../game.hpp"

class GridCGI : public CardGraphicItem {
public:
    GridCGI(const QString& fname, GetFlashCard& fc, QGraphicsItem* parent = nullptr);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w);

private:
    bool ontop;
};
