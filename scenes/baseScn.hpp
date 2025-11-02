#pragma once
#include <QWidget>

class BaseScene : public QWidget {
    Q_OBJECT
public:
    BaseScene();
    //~BaseScene() {}
//     void paintEvent(QPaintEvent*) override {}
//     void resizeEvent(QResizeEvent *event) override {};
    void keyPressEvent(QKeyEvent* event) override;
public slots:
    virtual void dialogOpen() {}  // For pausing timers and such
    virtual void dialogClose() {} // For resuming timers and such
};

