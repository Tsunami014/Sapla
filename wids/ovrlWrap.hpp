#pragma once
#include <QWidget>


// NOTE: This entire class was chatGPT'd. Saved me about 10 hours
class OverlayWrapper : public QWidget {
public:
    OverlayWrapper(QWidget* parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
    }

    // Call this when changing scenes (or if the overlay hides)
    void clearForwarding();

protected:
    bool event(QEvent* ev) override;

private:
    void forwardMouse(QWidget* target, QMouseEvent* src, const QPoint& globalPos);

    void transitionHover(QWidget* newTarget);

    void handleLeaveAll();

    QWidget* hoverTarget_ = nullptr;
    QWidget* pressTarget_ = nullptr;
};

