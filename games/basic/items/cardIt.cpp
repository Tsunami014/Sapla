#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include "gridLayout.hpp"
#include "overlay.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(layout l, BaseSideRend* fr, BaseSideRend* bk, QGraphicsItem* parent) : SvgGraphicItem(l.fname, parent) {
    lay = l;
    front = fr;
    back = bk;
    init();
}
CardGraphicItem::CardGraphicItem(layout l, const FlashCard& fc, QGraphicsItem* parent) : SvgGraphicItem(l.fname, parent) {
    lay = l;
    front = fc.getSide(0);
    back = fc.getSide(1);
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
    CardGraphicItem nCGI(lay, other);
    return nCGI == *this;
}

void CardGraphicItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    bool oldHover = hovering;
    if (contains(event->pos()) && !((PlayScene*)MG->curScene)->hasOverlay()) {
        hovering = true;
        setCursor(Qt::PointingHandCursor);
    } else {
        if (hovering) {
            unsetCursor();
        }
        hovering = false;
    }
    if (oldHover != hovering) { update(); }
    SvgGraphicItem::hoverMoveEvent(event);
}
void CardGraphicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hovering = false;
    update();
}
void CardGraphicItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (contains(event->pos())) {
        if (side == 0) {
            PlayScene* curS = (PlayScene*)ln->MG->curScene;
            curS->pauseTimer();
            unsetCursor();
            setParentItem(nullptr);  // So the item can be placed on the very top
            setPos(mapToScene(QPointF(0, 0)));
            setZValue(4);
            auto* overl = new Overlay();
            ln->MScene->addItem(overl);
            overl->setZValue(3);
            curS->setOverlay(overl);
            side = 255;  // TODO: Animations
            hovering = false;
            curS->main->updateAllChildren();
        }
    }
}

void CardGraphicItem::finish() {
    PlayScene* curS = (PlayScene*)MG->curScene;
    curS->removeOverlay();
    curS->main->removeItem(this);
    curS->main->update();
    scene()->removeItem(this);
    curS->resumeTimer();
    curS->main->updateAllChildren();
}

void CardGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    QImage img(rect.width(), rect.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p1(&img);
    renderer->render(&p1, QRectF(QPointF(0, 0), rect.size()));
    p1.end();

    if (hovering || zValue() == 3) {
        // Create a colorized outline version
        QImage outline = img;
        QPainter p2(&outline);
        p2.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p2.fillRect(outline.rect(), Qt::yellow);
        p2.end();

        painter->drawImage(QPointF(rect.left()-2, rect.top()-2), outline.scaled(rect.width()+4, rect.height()+4, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    painter->drawImage(rect.topLeft(), img);

    if (side == 0) {
        if (!((PlayScene*)MG->curScene)->hasOverlay()) {
            front->render(painter, rect);
        }
    } else {
        if (side == 255) {
            back->render(painter, rect);
        } else {
            // TODO: Partial flipped states
        }
    }
}
