#include "gridLayout.hpp"
#include "cardIt.hpp"
#include "../main.hpp"

GLayoutGraphicItem::GLayoutGraphicItem(QGraphicsItem* parent)
    : QGraphicsItem(parent) {
        if (!MTrenderer) {
            MTrenderer = std::make_unique<QSvgRenderer>(QString(":/assets/CardLayouts/emptyIt.svg"));
        }
    }

void GLayoutGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    float colWid = rect.width()/Cols;
    float rowHei = rect.height()/2;
    for (int i = 0; i < Cols; i++) {
        for (int j = 0; j < 2; j++) {
            int x = rect.x() + i*colWid;
            int y = rect.y() + j*rowHei;
            MTrenderer->render(painter, QRectF(x, y, colWid, rowHei));
        }
    }
}

QRectF GLayoutGraphicItem::boundingRect() const {
    return rect;
}

void GLayoutGraphicItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    rect = newRect;
    // Reduce height by 0.1 on both sides
    rect.setY(rect.y() + rect.height()*0.1);
    rect.setHeight(rect.height()*0.8);

    float colWid = rect.width()/Cols;
    float rowHei = rect.height()/2;
    for (gridItem& it : grid) {
        int hei;
        if (it.lay.botWid == -1) {
            hei = 1;
        } else {
            hei = 2;
        }
        it.item->setRect(QRectF(
            rect.x() + colWid * it.x, rect.y() + colWid * it.y, 
            colWid * qMax(it.lay.botWid, it.lay.topWid), rowHei * hei
        ));
    }
}

bool GLayoutGraphicItem::addItem(CardGraphicItem* item) {
    uint8_t nx = 0;
    uint8_t ny = 0;
    // TODO: Find the right spot here, and return false if failed
    gridItem val;
    val.item = item;
    val.lay = item->lay;
    val.x = nx;
    val.y = ny;
    grid.push_back(val);
    MScene->addItem(item);
    setRect(rect);
    return true;
}

