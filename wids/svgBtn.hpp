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

    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    QSize sizeHint() const override;

    void paintEvent(QPaintEvent* event) override;

signals:
    void clicked();

private:
    void init();
    QSvgRenderer* rend;
    bool hover;
};
