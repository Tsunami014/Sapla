#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QWidget>
#include <QGraphicsView>
#include <iostream>
#include "main.hpp"
#include "cards/cardTyps.hpp"
#include "cards/getCards.hpp"

BetterScene* MScene = nullptr;
MainGame* MG = nullptr;


class BetterView : public QGraphicsView {
public:
    BetterView(QGraphicsScene* scene) : QGraphicsView(scene) {}

protected:
    void resizeEvent(QResizeEvent* event) override {
        QGraphicsView::resizeEvent(event);
        QRectF newRect = QRectF(QPointF(0, 0), viewport()->size());
        MScene->setSceneRect(newRect);
        MG->resizeEvent(newRect);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    std::cout << getPath().toStdString() << "\n";
    std::cout.flush();
    registerCardTypes();
    initCards();
    MScene = new BetterScene();
    BetterView* view = new BetterView(MScene);
    MG = new MainGame();
    MG->initScene();

    view->showMaximized();

    view->show();
    return app.exec();
}
