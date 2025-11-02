#pragma once
#include <QWidget>
#include <QSvgRenderer>
#include <QPainter>

class SvgWidget : public QWidget {
    Q_OBJECT
public:
    SvgWidget(const QString& pth, QWidget* parent = nullptr)
        : QWidget(parent), rend(new QSvgRenderer(pth, this)) {}
    SvgWidget(QWidget* parent = nullptr)
        : QWidget(parent), rend(new QSvgRenderer(this)) {}

    QSvgRenderer* renderer() { return rend; }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        rend->render(&painter, rect());
    }

private:
    QSvgRenderer* rend;
};
