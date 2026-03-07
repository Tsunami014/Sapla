#pragma once
#include "items/cardIt.hpp"

class GridCGI : public CardGraphicItem {
    Q_OBJECT
public:
    GridCGI(const QString& fname, GetFlashCard& fc, QGraphicsItem* parent = nullptr);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) override;

private:
    void onflip(bool back) override;
    bool ontop;
};
