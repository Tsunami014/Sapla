#pragma once
#include "../base/svgUtils.hpp"
#include <QSvgRenderer>
#include <QLabel>

class SvgBtn : public QLabel, public SvgUtils {
    Q_OBJECT
public:
    SvgBtn(QWidget* parent = nullptr);
    SvgBtn(const QString& path, QWidget* parent = nullptr);

    QRect getRect() override { return rect(); }

    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

signals:
    void clicked();

private:
    void init();
    void fixPadding();
};
