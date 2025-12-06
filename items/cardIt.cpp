#include "cardIt.hpp"
#include "../base/font.hpp"
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(const QString& fname, GetFlashCard flashc, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(fname), side(0), fc(std::move(flashc)),
      front(fc->getSideHtml(SIDE_FRONT)), back(fc->getSideHtml(SIDE_BACK)), txt() {
        side = 0;
        setAcceptHoverEvents(true);
        txt.setTextFormat(Qt::RichText);
        txt.setWordWrap(true);
        txt.setAlignment(Qt::AlignCenter);
        txt.setMargin(4);
        txt.setStyleSheet("color: black;");
        txt.setAttribute(Qt::WA_TranslucentBackground);
        txt.setFont(getFont(1.5));
    }

bool CardGraphicItem::operator==(const CardGraphicItem& other) const {
    return fc.get() == other.fc.get();
    }
    bool CardGraphicItem::operator==(const FlashCard& other) const {
    return *fc == other;
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
            side = 255;  // TODO: Animations
            hover = false;
        }
    }
}

void CardGraphicItem::paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    if (side == 0) {
        txt.setText(front);
    } else {
        if (side == 255) {
            txt.setText(back);
        } else {
            // TODO: Partial flipped states
        }
    }
    paintSvg(p);

    const int adjustment = 40;
    QRectF newr = rect.adjusted(adjustment, adjustment, -adjustment, -adjustment);
    txt.resize(newr.size().toSize());
    p->save();
    p->translate(newr.topLeft());
    txt.render(p);
    p->restore();
}
