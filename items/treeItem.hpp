#pragma once
#include "../base/rectIt.hpp"
#include "progress.hpp"
#include <QSvgRenderer>

QByteArray rendTreePhase(int phase);

class Tree : public RectItem {
public:
    bool grow(double amount, bool canChangePhase = true);
    bool isDone();
    void nextPhase();
    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    void restart();

    static Tree& getTree() {
        static Tree inst;
        return inst;
    }

    // Stop copying or moving
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    Tree(Tree&&) = delete;
    Tree& operator=(Tree&&) = delete;

protected:
    Tree();
    double growth;
    double toNext;
    ProgressBarItem pb;
    static QSvgRenderer* baseRend;
    QSvgRenderer* treeRend;

private:
    int phase;
    bool isSmall;
};
