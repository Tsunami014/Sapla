#pragma once
#include <QAbstractButton>
#include <QSvgRenderer>
#include <QLabel>

class SvgBtn : public QAbstractButton {
    Q_OBJECT
public:
    SvgBtn(const QString& path, QWidget* parent = nullptr);
    bool hasHeightForWidth() const override { return true; };
    int heightForWidth(int w) const override;
    QSize sizeHint() const override;

    void setText(const QString& txt) {
        QAbstractButton::setText(txt);
        updateGeometry();    update();
    }

    void setFont(const QFont& f) {
        QAbstractButton::setFont(f);
        updateGeometry();  update();
    }
protected:
    void paintEvent(QPaintEvent*) override;
    QSvgRenderer* rend;
};

