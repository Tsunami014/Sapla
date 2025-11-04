#pragma once
#include <QWidget>
#include <QKeyEvent>

class BaseScene : public QWidget {
    Q_OBJECT
public:
    BaseScene();
    ~BaseScene();
//     void paintEvent(QPaintEvent*) override {}
//     void resizeEvent(QResizeEvent *event) override {};
    virtual bool keyEv(QKeyEvent* event);
public slots:
    virtual void dialogOpen() {}  // For pausing timers and such
    virtual void dialogClose() {} // For resuming timers and such

private:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

