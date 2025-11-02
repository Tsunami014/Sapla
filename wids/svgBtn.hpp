#pragma once
#include <QSvgRenderer>
#include <QLabel>

class SvgBtn : public QLabel
{
    Q_OBJECT
public:
    SvgBtn(QWidget* parent = nullptr);
    SvgBtn(const QString& path, QWidget* parent = nullptr);
    void setSvg(const QString& path);

    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

signals:
    void clicked();

private:
    void init();
    bool touching(const QPointF& pos);
    QSvgRenderer* rend;
    bool hover;
};
