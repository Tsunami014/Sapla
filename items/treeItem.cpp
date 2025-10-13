#include "treeItem.hpp"
// New imports
#include <QPainter>
#include <QRandomGenerator>

// New swap
//#define MAX_PHASE 5
#define MAX_PHASE 1

QSvgRenderer* Tree::baseRend = NULL;

QString Tree::fullTxt = ""; // New

Tree::Tree(QGraphicsItem* parent) : SvgGraphicItem(parent), pb(this) {
    if (!baseRend) baseRend = new QSvgRenderer(QStringLiteral(":/assets/treeGround.svg"));
    phase = 0;
    growth = 0;
    toNext = 50;
    nextPhase();
}

void Tree::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    qreal hei = newRect.height()/2;
    qreal wid = hei/2;
    rect = {
        newRect.right()-wid, ((newRect.y()+newRect.height()) - hei)/2, 
        wid, hei
    };
    pb.setRect({
        rect.x(), rect.y() + rect.height()*1.1,
        rect.width(), newRect.height()*0.04
    });
}

bool Tree::grow(double amount) {
    growth += amount;
    if (growth < 0) {
        growth = 0;
    }
    if (growth >= toNext) {
        growth -= toNext;
        if (phase+1 >= MAX_PHASE) {
            return false;
        }
        nextPhase();
    }
    pb.setValue(growth/toNext);
    return true;
}
void Tree::nextPhase() {
    delete renderer;
    renderer = new QSvgRenderer(QString::fromStdString(
        ":/assets/TreeParts/phase" + std::to_string(++phase) + ".svg"
    ));
    isSmall = renderer->defaultSize().height() == 16;
    toNext = ((toNext - 50)/100 + 1)*100 + 50;
    toNext = 2000;  // New

    if (phase == MAX_PHASE) pb.hide();
}
void Tree::lastPhase() {
    phase = MAX_PHASE - 1;
    nextPhase();
    growth = toNext;  // New
}

void Tree::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    // All before return is new
    painter->setPen(Qt::black);
    int goodChars = (growth/toNext) * fullTxt.length();
    QString res = fullTxt.sliced(0, goodChars);
    if (goodChars > 0 && goodChars < fullTxt.length()) {
        res = "[" + res + "]\n";
    }
    for (int _ = 0; _ < fullTxt.length() - goodChars; _++) {
        int code;
        code = QRandomGenerator::global()->bounded(33, 127);
        res += QChar(code);
    }
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    painter->drawText(rect, res, opt);
    return;
    // Because the svg is smaller than the bounding rect, and I really want to show them off
    QRectF fitR = rect.adjusted(-rect.width()*0.1, -rect.height()*0.1, rect.width()*0.1, rect.height()*0.1);
    QRectF sTreeR{
        fitR.x(), fitR.y() + fitR.height()/2, fitR.width(), fitR.height()/2
    };
    baseRend->render(painter, sTreeR);
    QRectF drawR;
    if (isSmall) {
        drawR = sTreeR;
    } else {
        drawR = fitR;
    }
    renderer->render(painter, drawR);
}

// New stuff:
void Tree::changeTxt() {
    fullTxt = "This is a very long test testing some very long text inside this very short box.";
}

