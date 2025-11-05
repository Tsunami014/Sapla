#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(const QString& fname, BaseSideRend* fr, BaseSideRend* bk, QGraphicsItem* parent) : RectItem(parent), SvgUtils(fname) {
    front = fr;
    back = bk;
    init();
}
CardGraphicItem::CardGraphicItem(const QString& fname, const FlashCard& fc, QGraphicsItem* parent) : RectItem(parent), SvgUtils(fname) {
    front = fc.getSide(SIDE_FRONT);
    back = fc.getSide(SIDE_BACK);
    init();
}
void CardGraphicItem::init() {
    side = 0;
    setAcceptHoverEvents(true);
}
CardGraphicItem::~CardGraphicItem() {
    delete front;
    delete back;
}

bool CardGraphicItem::operator==(const CardGraphicItem& other) const {
    return *front == *other.front && *back == *other.back;
}
bool CardGraphicItem::operator==(const FlashCard& other) const {
    auto* fr = other.getSide(SIDE_FRONT);
    auto* bk = other.getSide(SIDE_BACK);
    bool eq = *front == *fr && *back == *bk;
    delete fr; delete bk;
    return eq;
}

void CardGraphicItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    bool oldHover = hover;
    if (touching(event->pos())) {
        hover = true;
        setCursor(Qt::PointingHandCursor);
    } else {
        if (hover) {
            unsetCursor();
        }
        hover = false;
    }
    if (oldHover != hover) { update(); }
    RectItem::hoverMoveEvent(event);
}
void CardGraphicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hover = false;
    update();
}
void CardGraphicItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (touching(event->pos())) {
        if (side == 0) {
            unsetCursor();
            setZValue(4);
            side = 255;  // TODO: Animations
            hover = false;
        }
    }
}

void CardGraphicItem::finish() {
    PlayScene* curS = (PlayScene*)MG->curScene;
    scene()->removeItem(this);
}

void CardGraphicItem::paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    paintSvg(p);

    if (side == 0) {
        front->render(p, rect);
    } else {
        if (side == 255) {
            back->render(p, rect);
        } else {
            // TODO: Partial flipped states
        }
    }
}
