#pragma once
#include "../base/rectIt.hpp"
#include "../base/svgUtils.hpp"
#include "../notes/getNotes.hpp"
#include "../notes/cardList.hpp"
#include <QLabel>

class CardGraphicItem : public RectItem, public SvgUtils {
public:
    CardGraphicItem(const QString& fname, GetFlashCard fc, QGraphicsItem* parent = nullptr);   
    ~CardGraphicItem();

    QRect getRect() override { return rect.toRect(); }

    bool operator==(const CardGraphicItem& other) const;
    bool operator==(const FlashCard& other) const;

    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    uint8_t side;  // 0 if front, 255 if back, anywhere else for a transition effect between the 2

    GetFlashCard fc;

protected:
    QLabel txt;
    QString front;
    QString back;
};

