#include "cardIt.hpp"
#include "../base/font.hpp"
#include "../base/svgRend.hpp"
#include "../base/seedrng.hpp"
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>

CardGraphicItem::CardGraphicItem(const QString& fname, GetFlashCard& flashc, float fontsze, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(fname), side(0), fc(std::move(flashc)), txt() {
        setSeed();
        front = fc->getSideHtml(SIDE_FRONT);
        back = fc->getSideHtml(SIDE_BACK);
        setAcceptHoverEvents(true);
        txt.setTextFormat(Qt::RichText);
        txt.setWordWrap(true);
        txt.setAlignment(Qt::AlignCenter);
        txt.setMargin(4);
        txt.setStyleSheet("color: black;");
        txt.setAttribute(Qt::WA_TranslucentBackground);
        txt.setFont(getFont(fontsze));
        onflip(false);
    }
void CardGraphicItem::onflip(bool back) {
    // TODO: Animations
    if (back) {
        side = 255;
        hlcol = getCol("adark", 100, 50, 100);
    } else {
        hlcol = getCol("alight", 100, 50, 100);
        side = 0;
    }
    emit flipped(back);
    update();
}
CardGraphicItem::~CardGraphicItem() {
    txt.deleteLater();
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
void CardGraphicItem::focusInEvent(QFocusEvent *event) {
    hover = true;
    if (event != nullptr) QGraphicsItem::focusInEvent(event);
    update();
}
void CardGraphicItem::focusOutEvent(QFocusEvent *event) {
    hover = false;
    if (event != nullptr) QGraphicsItem::focusOutEvent(event);
    update();
}
void CardGraphicItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (touching(event->pos())) flip();
}
void CardGraphicItem::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter) {
        flip();
    } else {
        QGraphicsItem::keyPressEvent(event);
    }
}

void CardGraphicItem::flip() {
    if (side == 0) {
        onflip(true);
    } else if (side == 255) {
        onflip(false);
    }
}

void CardGraphicItem::paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    if (side == 0) {
        txt.setText(front);
    } else {
        if (side == 255) {
            txt.setText(back);
            hover = true;
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
