#include "gridLayout.hpp"
#include "cardIt.hpp"
#include "../main.hpp"
#include <QRandomGenerator>

GLayoutGraphicItem::GLayoutGraphicItem(QGraphicsItem* parent)
    : MyGraphicsItem(parent) {
        if (!MTrenderer) {
            MTrenderer = std::make_unique<QSvgRenderer>(QString(":/assets/CardLayouts/emptyIt.svg"));
        }
    }

bool GLayoutGraphicItem::isGood(int col, int row) {
    for (gridItem& it : grid) {
        int itx = it.x;
        int itWid;
        if (row == 1 && it.y == 0) {
            if (it.lay.botWid == -1) {
                continue;
            }
            itx += it.lay.botOffs;
            itWid = it.lay.botWid;
        } else if (row == 0 && it.y == 1) {
            continue;
        } else {
            itWid = it.lay.topWid;
        }
        if (col >= itx && col < itx + itWid) {
            return false;
        }
    }
    return true;
}

void GLayoutGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    float colWid = rect.width()/Cols;
    float rowHei = rect.height()/2;
    for (int i = 0; i < Cols; i++) {
        for (int j = 0; j < 2; j++) {
            int x = rect.x() + i*colWid;
            int y = rect.y() + j*rowHei;
            if (isGood(i, j)) {
                MTrenderer->render(painter, QRectF(x, y, colWid, rowHei));
            }
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
            rect.x() + colWid * it.x, rect.y() + rowHei * it.y, 
            colWid * qMax(it.lay.botWid, it.lay.topWid), rowHei * hei
        ));
    }
}

bool GLayoutGraphicItem::addItem(CardGraphicItem* item) {
    uint8_t loops = (item->lay.botWid == -1) + 1;
    uint8_t maxX = Cols + 1 - qMax(item->lay.topWid, item->lay.botWid);
    uint8_t nx = QRandomGenerator::global()->bounded(0, maxX);
    uint8_t ny = QRandomGenerator::global()->bounded(0, loops);
    for (uint8_t _ = 0; _ < loops; _++) {
        for (uint8_t offx = 0; offx < maxX; offx++) {
            if (isGood((nx + offx) % maxX, ny)) {  // TODO: Check if isGood for every square that makes up the object
                gridItem val;
                val.item = item;
                item->setParentItem(this);
                val.lay = item->lay;
                val.x = (nx + offx) % maxX;
                val.y = ny;
                grid.push_back(val);
                MScene->addItem(item);
                setRect(rect);
                update();
                return true;
            }
        }
        // loops can only be 0 or 1
        if (ny == 0) {
            ny = loops - 1;
        } else {
            ny = 0;
        }
    }
    return false;
}

void GLayoutGraphicItem::removeItem(CardGraphicItem* item) {
    grid.erase(
        std::remove_if(grid.begin(), grid.end(),
                       [item](gridItem& it) { return it.item == item; }),
        grid.end()
    );
}

