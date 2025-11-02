#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(const QString& fname, BaseSideRend* fr, BaseSideRend* bk, QGraphicsItem* parent) : RectItem(parent), rend(RenderSvg(fname)) {
    front = fr;
    back = bk;
    init();
}
CardGraphicItem::CardGraphicItem(const QString& fname, const FlashCard& fc, QGraphicsItem* parent) : RectItem(parent), rend(RenderSvg(fname)) {
    front = fc.getSide(SIDE_FRONT);
    back = fc.getSide(SIDE_BACK);
    init();
}
void CardGraphicItem::init() {
    side = 0;
    hovering = false;
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
    bool oldHover = hovering;
    if (contains(event->pos())) {
        hovering = true;
        setCursor(Qt::PointingHandCursor);
    } else {
        if (hovering) {
            unsetCursor();
        }
        hovering = false;
    }
    if (oldHover != hovering) { update(); }
    RectItem::hoverMoveEvent(event);
}
void CardGraphicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hovering = false;
    update();
}
void CardGraphicItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (contains(event->pos())) {
        if (side == 0) {
            PlayScene* curS = (PlayScene*)MG->curScene;
            unsetCursor();
            setZValue(4);
            side = 255;  // TODO: Animations
            hovering = false;
        }
    }
}

void CardGraphicItem::finish() {
    PlayScene* curS = (PlayScene*)MG->curScene;
    scene()->removeItem(this);
}

void CardGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    QImage img(rect.width(), rect.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p1(&img);
    rend.render(&p1, QRectF(QPointF(0, 0), rect.size()));
    p1.end();

    if (hovering || zValue() == 3) {
        // Create a colorized outline version
        QImage outline = img;
        QPainter p2(&outline);
        p2.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p2.fillRect(outline.rect(), Qt::yellow);
        p2.end();

        painter->drawImage(QPointF(-2, -2), outline.scaled(rect.width()+4, rect.height()+4, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    painter->drawImage(QPointF(0, 0), img);

    if (side == 0) {
        front->render(painter, rect);
    } else {
        if (side == 255) {
            back->render(painter, rect);
        } else {
            // TODO: Partial flipped states
        }
    }
}
