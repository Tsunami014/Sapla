#pragma once
#include "../log.hpp"
#include "../base/rectIt.hpp"
#include "progress.hpp"
#include <QSvgRenderer>

QByteArray rendTreePhase(int phase);

struct _treeInfo {
    _treeInfo();
    bool grow(double amount, bool canChangePhase = true);
    bool isDone();
    void nextPhase();
    void restart();
    float percentage();
    int getPhase();
private:
    double growth;
    double toNext;
    int phase;
};
extern _treeInfo TreeInfo;

class Tree : public RectItem {
public:
    Tree(QGraphicsItem* parent = nullptr);
    ~Tree();
    bool grow(double amount, bool canChangePhase = true);
    void nextPhase();
    void restart();

    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    // Stop copying or moving
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    Tree(Tree&&) = delete;
    Tree& operator=(Tree&&) = delete;

protected:
    void nextPhase(bool updateInfo);
    ProgressBarItem pb;
    QSvgRenderer* baseRend;
    QSvgRenderer* treeRend;

private:
    bool isSmall;
};
