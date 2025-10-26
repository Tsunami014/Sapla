#pragma once
#include "../game.hpp"

class CardGraphicItem : public SvgGraphicItem {
public:
    CardGraphicItem(const QString& fname, BaseSideRend* front, BaseSideRend* back, QGraphicsItem* parent = nullptr);   
    CardGraphicItem(const QString& fname, const FlashCard& fc, QGraphicsItem* parent = nullptr);   
    ~CardGraphicItem();

    bool operator==(const CardGraphicItem& other) const;
    bool operator==(const FlashCard& other) const;

    void finish();

    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    uint8_t side;  // 0 if front, 255 if back, anywhere else for a transition effect between the 2
protected:
    void init();
    BaseSideRend* front;
    BaseSideRend* back;

    bool hovering;
};

