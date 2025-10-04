#include "../main.hpp"
#include "../scenes/playScn.hpp"
#include "cardIt.hpp"
#include "gridLayout.hpp"
#include "overlay.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(layout l, BaseSideRend* fr, BaseSideRend* bk, QGraphicsItem* parent) : SvgGraphicItem(l.fname, parent) {
    lay = l;
    front = fr;
    back = bk;
    side = 0;

    hovering = false;
    setAcceptHoverEvents(true);
}

CardGraphicItem::~CardGraphicItem() {
    delete front;
    delete back;
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
        PlayScene* curS = (PlayScene*)MG->curScene;
        if (side == 0) {
            setZValue(3);
            auto* overl = new Overlay();
            MScene->addItem(overl);
            overl->setZValue(2);
            curS->setOverlay(overl);
            side = 255;  // TODO: Animations
        } else if (side == 255) {
            curS->removeOverlay();
            curS->main->removeItem(this);
            curS->main->update();
            scene()->removeItem(this);
        }
    }
}

void CardGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    // <chatGPT>
    QImage img(rect.width(), rect.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p1(&img);
    renderer->render(&p1);
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

    // </chatGPT>

    // TODO: Partial flipped states
    if (side == 0) {
        front->render(painter, rect);
    } else {
        back->render(painter, rect);
    }
}
